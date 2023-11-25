#include "VulkanDevice.h"
#include "engine/MemoryDebugging.h"

#include <engine/ConsoleLog.h>
#include <set>

bool VulkanDevice::QueueFamilyIndices::IsComplete() const
{
	return graphicsFamily.has_value() && presentFamily.has_value();
}

VulkanDevice::VulkanDevice(vk::Instance& instance, vk::SurfaceKHR& surface)
{
	// Enumerate physical devices and choose one (add device selection logic based on your requirements)
	auto physicalDevices = instance.enumeratePhysicalDevices();

	if (physicalDevices.empty())
	{
		SLIME_ERROR("Failed to find GPUs with Vulkan support");
	}

	// Find the best physical device based on your criteria
	for (const auto& device : physicalDevices)
	{
		if (IsDeviceSuitable(device, surface))
		{
			m_physicalDevice = device;
			break;
		}
	}

	if (!m_physicalDevice)
	{
		SLIME_ERROR("Failed to find a suitable GPU");
	}

	// Create logical device
	std::vector<vk::QueueFamilyProperties> queueFamilies = m_physicalDevice.getQueueFamilyProperties();
	// Loop through all the queue families
	for (uint32_t i = 0; i < queueFamilies.size(); i++)
	{
		// Get the queue family properties
		vk::QueueFamilyProperties queueFamilyProperty = queueFamilies[i];

		// Check if the queue family supports graphics
		if ((queueFamilyProperty.queueFlags & vk::QueueFlagBits::eGraphics) && !IsQueueFamilyIndexSet(QueueType::GRAPHICS))
		{
			SetQueueFamilyIndex(QueueType::GRAPHICS, i);
			continue;
		}

		// Check if the queue family supports presentation
		if (m_physicalDevice.getSurfaceSupportKHR(i, surface) && !IsQueueFamilyIndexSet(QueueType::PRESENT))
		{
			SetQueueFamilyIndex(QueueType::PRESENT, i);
			continue;
		}

		// Check if the queue family supports compute
		if ((queueFamilyProperty.queueFlags & vk::QueueFlagBits::eCompute) && !IsQueueFamilyIndexSet(QueueType::COMPUTE))
		{
			SetQueueFamilyIndex(QueueType::COMPUTE, i);
			continue;
		}

		// Check if the queue family supports transfer
		if ((queueFamilyProperty.queueFlags & vk::QueueFlagBits::eTransfer) && !IsQueueFamilyIndexSet(QueueType::TRANSFER))
		{
			SetQueueFamilyIndex(QueueType::TRANSFER, i);
			continue;
		}
	}

	// Check if all the queues have been set
	if (!IsQueueFamilyIndexSet(QueueType::GRAPHICS))
	{
		SLIME_ERROR("No graphics queue family index found!");
	}

	if (!IsQueueFamilyIndexSet(QueueType::PRESENT))
	{
		SLIME_ERROR("No present queue family index found!");
	}

	// Get the queue priorities
	std::vector<float> queuePriorities = { 1.0f };

	// Create the queue create infos
	std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
	queueCreateInfos.emplace_back(vk::DeviceQueueCreateFlags(), GetQueueFamilyIndex(QueueType::GRAPHICS), static_cast<uint32_t>(queuePriorities.size()), queuePriorities.data());
	queueCreateInfos.emplace_back(vk::DeviceQueueCreateFlags(), GetQueueFamilyIndex(QueueType::PRESENT), static_cast<uint32_t>(queuePriorities.size()), queuePriorities.data());

	if (IsQueueFamilyIndexSet(QueueType::TRANSFER))
		queueCreateInfos.emplace_back(vk::DeviceQueueCreateFlags(), GetQueueFamilyIndex(QueueType::TRANSFER), static_cast<uint32_t>(queuePriorities.size()), queuePriorities.data());

	if (IsQueueFamilyIndexSet(QueueType::COMPUTE))
		queueCreateInfos.emplace_back(vk::DeviceQueueCreateFlags(), GetQueueFamilyIndex(QueueType::COMPUTE), static_cast<uint32_t>(queuePriorities.size()), queuePriorities.data());

	// Create the device create info
	auto deviceCreateInfo = vk::DeviceCreateInfo(vk::DeviceCreateFlags(), static_cast<uint32_t>(queueCreateInfos.size()), queueCreateInfos.data());

	// Set the device features
	auto deviceFeatures = vk::PhysicalDeviceFeatures();

	// Check for tessellation and geometry shader support
	bool isTessellationShaderSupported = false;
	bool isGeometryShaderSupported     = false;
	const auto& availableExtensions    = m_physicalDevice.enumerateDeviceExtensionProperties();

	for (const auto& extension : availableExtensions)
	{
		if (strcmp(extension.extensionName, "VK_EXT_tessellation_shader") == 0)
			isTessellationShaderSupported = true;
		else if (strcmp(extension.extensionName, "VK_EXT_geometry_shader") == 0)
			isGeometryShaderSupported = true;

		// Break out of loop early if both extensions are supported
		if (isTessellationShaderSupported && isGeometryShaderSupported)
			break;
	}

	// Enable the corresponding features if supported
	if (isTessellationShaderSupported)
		deviceFeatures.tessellationShader = VK_TRUE;

	if (isGeometryShaderSupported)
		deviceFeatures.geometryShader = VK_TRUE;

	deviceCreateInfo.setPEnabledFeatures(&deviceFeatures);

	deviceCreateInfo.setEnabledExtensionCount(static_cast<uint32_t>(m_deviceExtensions.size()));
	deviceCreateInfo.setPpEnabledExtensionNames(m_deviceExtensions.data());

	// Create the logical device
	m_logicalDevice = m_physicalDevice.createDevice(deviceCreateInfo);

	// Get the queues
	SetQueue(QueueType::GRAPHICS, m_logicalDevice.getQueue(GetQueueFamilyIndex(QueueType::GRAPHICS), 0));
	SetQueue(QueueType::PRESENT, m_logicalDevice.getQueue(GetQueueFamilyIndex(QueueType::PRESENT), 0));

	if (IsQueueFamilyIndexSet(QueueType::TRANSFER))
		SetQueue(QueueType::TRANSFER, m_logicalDevice.getQueue(GetQueueFamilyIndex(QueueType::TRANSFER), 0));

	if (IsQueueFamilyIndexSet(QueueType::COMPUTE))
		SetQueue(QueueType::COMPUTE, m_logicalDevice.getQueue(GetQueueFamilyIndex(QueueType::COMPUTE), 0));

	m_graphicsCommandPool = CreateCommandPool(QueueType::GRAPHICS);
	m_presentCommandPool  = CreateCommandPool(QueueType::PRESENT);

	OutputDeviceProperties();
}

VulkanDevice::~VulkanDevice()
{
	m_logicalDevice.destroyCommandPool(m_graphicsCommandPool);
	m_logicalDevice.destroyCommandPool(m_presentCommandPool);
}

vk::PhysicalDevice VulkanDevice::GetPhysicalDevice()
{
	return m_physicalDevice;
}

vk::Device VulkanDevice::GetLogicalDevice()
{
	return m_logicalDevice;
}

bool VulkanDevice::IsDeviceSuitable(vk::PhysicalDevice device, vk::SurfaceKHR surface)
{
	// Check if the device supports the required queue families
	QueueFamilyIndices indices = FindQueueFamilies(device, surface);

	// Check if the device supports the required extensions
	bool extensionsSupported = CheckDeviceExtensionSupport(device);

	// Check if the device supports the required swap chain details
	bool swapChainAdequate = false;
	if (extensionsSupported)
	{
		SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device, surface);
		swapChainAdequate                        = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	// Check if the device supports the required features
	vk::PhysicalDeviceFeatures supportedFeatures = device.getFeatures();

	return indices.IsComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
}

bool VulkanDevice::CheckDeviceExtensionSupport(vk::PhysicalDevice device)
{
	// Get the available extensions
	auto availableExtensions = device.enumerateDeviceExtensionProperties();

	// Check if the required extensions are supported
	std::set<std::string> requiredExtensions(m_deviceExtensions.begin(), m_deviceExtensions.end());

	for (const auto& extension : availableExtensions)
	{
		requiredExtensions.erase(extension.extensionName.data());
	}

	return requiredExtensions.empty();
}

VulkanDevice::QueueFamilyIndices VulkanDevice::FindQueueFamilies(vk::PhysicalDevice device, vk::SurfaceKHR surface)
{
	QueueFamilyIndices indices;

	// Get the queue family properties
	auto queueFamilyProperties = device.getQueueFamilyProperties();

	for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilyProperties.size()); ++i)
	{
		if (queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics)
		{
			indices.graphicsFamily = i;
		}

		bool presentSupport = device.getSurfaceSupportKHR(i, surface);

		if (presentSupport)
		{
			indices.presentFamily = i;
		}

		if (indices.IsComplete())
		{
			break;
		}
	}

	return indices;
}

VulkanDevice::SwapChainSupportDetails VulkanDevice::QuerySwapChainSupport(vk::PhysicalDevice device, vk::SurfaceKHR surface)
{
	SwapChainSupportDetails details;

	// Get the surface capabilities
	details.capabilities = device.getSurfaceCapabilitiesKHR(surface);

	// Get the supported surface formats
	details.formats = device.getSurfaceFormatsKHR(surface);

	// Get the supported presentation modes
	details.presentModes = device.getSurfacePresentModesKHR(surface);

	return details;
}

vk::Queue VulkanDevice::GetQueue(QueueType queueType) const
{
	switch (queueType)
	{
	case QueueType::GRAPHICS: return m_graphicsQueue;
	case QueueType::PRESENT: return m_presentQueue;
	case QueueType::COMPUTE: return m_computeQueue;
	case QueueType::TRANSFER: return m_transferQueue;
	default: return nullptr;
	}
}

uint32_t VulkanDevice::GetQueueIndex(QueueType queueType) const
{
	switch (queueType)
	{
	case QueueType::GRAPHICS: return m_graphicsQueueIndex;
	case QueueType::PRESENT: return m_presentQueueIndex;
	case QueueType::COMPUTE: return m_computeQueueIndex;
	case QueueType::TRANSFER: return m_transferQueueIndex;
	default: return 0;
	}
}

uint32_t VulkanDevice::GetQueueFamilyIndex(QueueType queueType) const
{
	switch (queueType)
	{
	case QueueType::GRAPHICS: return m_graphicsQueueFamilyIndex;
	case QueueType::PRESENT: return m_presentQueueFamilyIndex;
	case QueueType::COMPUTE: return m_computeQueueFamilyIndex;
	case QueueType::TRANSFER: return m_transferQueueFamilyIndex;
	default: return 0;
	}
}

void VulkanDevice::SetQueue(QueueType queueType, vk::Queue queue)
{
	switch (queueType)
	{
	case QueueType::GRAPHICS: m_graphicsQueue = queue; break;
	case QueueType::PRESENT: m_presentQueue = queue; break;
	case QueueType::COMPUTE: m_computeQueue = queue; break;
	case QueueType::TRANSFER: m_transferQueue = queue; break;
	default: break;
	}
}

void VulkanDevice::SetQueueIndex(QueueType queueType, uint32_t queueIndex)
{
	switch (queueType)
	{
	case QueueType::GRAPHICS: m_graphicsQueueIndex = queueIndex; break;
	case QueueType::PRESENT: m_presentQueueIndex = queueIndex; break;
	case QueueType::COMPUTE: m_computeQueueIndex = queueIndex; break;
	case QueueType::TRANSFER: m_transferQueueIndex = queueIndex; break;
	default: break;
	}
}

void VulkanDevice::SetQueueFamilyIndex(QueueType queueType, uint32_t queueFamilyIndex)
{
	switch (queueType)
	{
	case QueueType::GRAPHICS: m_graphicsQueueFamilyIndex = queueFamilyIndex; break;
	case QueueType::PRESENT: m_presentQueueFamilyIndex = queueFamilyIndex; break;
	case QueueType::COMPUTE: m_computeQueueFamilyIndex = queueFamilyIndex; break;
	case QueueType::TRANSFER: m_transferQueueFamilyIndex = queueFamilyIndex; break;
	default: break;
	}
}

bool VulkanDevice::IsQueueFamilyIndexSet(QueueType queueType) const
{
	switch (queueType)
	{
	case QueueType::GRAPHICS: return m_graphicsQueueFamilyIndex != UINT32_MAX;
	case QueueType::PRESENT: return m_presentQueueFamilyIndex != UINT32_MAX;
	case QueueType::COMPUTE: return m_computeQueueFamilyIndex != UINT32_MAX;
	case QueueType::TRANSFER: return m_transferQueueFamilyIndex != UINT32_MAX;
	default: return false;
	}
}

// DEBUG FUNCTIONS
const char* VulkanDevice::BoolToString(bool value)
{
	return value ? "True" : "False";
}

const char* VulkanDevice::GetVendorName(uint32_t vendorID)
{
	switch (static_cast<VendorID>(vendorID))
	{
	case VendorID::AMD: return "AMD";
	case VendorID::ImgTec: return "ImgTec";
	case VendorID::NVIDIA: return "NVIDIA";
	case VendorID::ARM: return "ARM";
	case VendorID::Qualcomm: return "Qualcomm";
	case VendorID::INTEL: return "INTEL";
	default: return "Unknown";
	}
}

const char* VulkanDevice::GetDeviceTypeName(vk::PhysicalDeviceType deviceType)
{
	switch (deviceType)
	{
	case vk::PhysicalDeviceType::eCpu: return "CPU";
	case vk::PhysicalDeviceType::eDiscreteGpu: return "Discrete GPU";
	case vk::PhysicalDeviceType::eIntegratedGpu: return "Integrated GPU";
	case vk::PhysicalDeviceType::eVirtualGpu: return "Virtual GPU";
	case vk::PhysicalDeviceType::eOther: return "Unknown";
	default: return "Unknown";
	}
}

vk::CommandPool VulkanDevice::CreateCommandPool(QueueType queueType)
{
	vk::CommandPoolCreateInfo poolInfo({}, GetQueueFamilyIndex(queueType));
	return m_logicalDevice.createCommandPool(poolInfo);
}

std::string VulkanDevice::GetFormattedApiVersion(uint32_t apiVersion)
{
	return std::to_string(VK_VERSION_MAJOR(apiVersion)) + "." + std::to_string(VK_VERSION_MINOR(apiVersion)) + "." + std::to_string(VK_VERSION_PATCH(apiVersion));
}

void VulkanDevice::AddValueToBuffer(const char* name, auto value)
{
	std::cout << std::setw(34) << std::left << name << ": " << value << "\n";
}

void VulkanDevice::OutputDeviceProperties()
{
	// PhysicalDevice
	vk::PhysicalDeviceProperties deviceProperties             = m_physicalDevice.getProperties();
	vk::PhysicalDeviceFeatures supportedFeatures              = m_physicalDevice.getFeatures();
	vk::PhysicalDeviceMemoryProperties deviceMemoryProperties = m_physicalDevice.getMemoryProperties();
	const char* deviceName                                    = deviceProperties.deviceName;

	const char* vendorName = GetVendorName(deviceProperties.vendorID);
	const char* deviceType = GetDeviceTypeName(deviceProperties.deviceType);

	std::cout << std::flush << "\nSelected Physical Device:\n";
	AddValueToBuffer("  Device Name", deviceName);
	AddValueToBuffer("  API Version", GetFormattedApiVersion(deviceProperties.apiVersion));
	AddValueToBuffer("  Driver Version", deviceProperties.driverVersion);
	AddValueToBuffer("  Vendor", vendorName);
	AddValueToBuffer("  Device ID", deviceProperties.deviceID);
	AddValueToBuffer("  Device Type", deviceType);
	std::cout << std::flush;

	std::cout << "\n  Physical Device Limits:\n";
	AddValueToBuffer("    - Max Image Dimension 1D", deviceProperties.limits.maxImageDimension1D);
	AddValueToBuffer("    - Max Image Dimension 2D", deviceProperties.limits.maxImageDimension2D);
	AddValueToBuffer("    - Max Image Dimension 3D", deviceProperties.limits.maxImageDimension3D);
	AddValueToBuffer("    - Max Image Dimension Cube", deviceProperties.limits.maxImageDimensionCube);
	AddValueToBuffer("    - Max Image Array Layers", deviceProperties.limits.maxImageArrayLayers);
	AddValueToBuffer("    - Max Texel Buffer Elements", deviceProperties.limits.maxTexelBufferElements);
	AddValueToBuffer("    - Max Uniform Buffer Range", deviceProperties.limits.maxUniformBufferRange);
	AddValueToBuffer("    - Max Storage Buffer Range", deviceProperties.limits.maxStorageBufferRange);
	AddValueToBuffer("    - Max Push Constants Size", deviceProperties.limits.maxPushConstantsSize);
	AddValueToBuffer("    - Max Memory Allocation Count", deviceProperties.limits.maxMemoryAllocationCount);
	AddValueToBuffer("    - Max Sampler Allocation Count", deviceProperties.limits.maxSamplerAllocationCount);
	std::cout << std::flush;

	std::cout << "\n  Physical Device Memory Properties:\n";
	AddValueToBuffer("    - Memory Type Count", deviceMemoryProperties.memoryTypeCount);
	AddValueToBuffer("    - Memory Heap Count", deviceMemoryProperties.memoryHeapCount);
	std::cout << std::flush;

	std::cout << "\n  Supported Vulkan Features:\n";
	AddValueToBuffer("    - robustBufferAccess", BoolToString(supportedFeatures.robustBufferAccess));
	AddValueToBuffer("    - fullDrawIndexUint32", BoolToString(supportedFeatures.fullDrawIndexUint32));
	AddValueToBuffer("    - imageCubeArray", BoolToString(supportedFeatures.imageCubeArray));
	AddValueToBuffer("    - independentBlend", BoolToString(supportedFeatures.independentBlend));
	AddValueToBuffer("    - geometryShader", BoolToString(supportedFeatures.geometryShader));
	AddValueToBuffer("    - tessellationShader", BoolToString(supportedFeatures.tessellationShader));
	AddValueToBuffer("    - sampleRateShading", BoolToString(supportedFeatures.sampleRateShading));
	AddValueToBuffer("    - dualSrcBlend", BoolToString(supportedFeatures.dualSrcBlend));
	AddValueToBuffer("    - logicOp", BoolToString(supportedFeatures.logicOp));
	AddValueToBuffer("    - multiDrawIndirect", BoolToString(supportedFeatures.multiDrawIndirect));
	AddValueToBuffer("    - drawIndirectFirstInstance", BoolToString(supportedFeatures.drawIndirectFirstInstance));
	AddValueToBuffer("    - depthClamp", BoolToString(supportedFeatures.depthClamp));

	std::cout << std::flush << std::endl;

	// LogicalDevice
	std::cout << "\nCreated Logical Device:\n";

	// Query queue family properties
	uint32_t queueFamilyCount = 0;
	m_physicalDevice.getQueueFamilyProperties(&queueFamilyCount, nullptr);
	std::vector<vk::QueueFamilyProperties> queueFamilies(queueFamilyCount);
	m_physicalDevice.getQueueFamilyProperties(&queueFamilyCount, queueFamilies.data());

	std::cout << "  Queue Families:\n";
	for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilies.size()); ++i)
	{
		std::cout << "    Family " << i << ":\n";
		AddValueToBuffer("      - Queue Count", queueFamilies[i].queueCount);
		AddValueToBuffer("      - Timestamp Valid Bits", queueFamilies[i].timestampValidBits);

		std::cout << std::flush << std::endl;
	}
	std::cout << std::flush << std::endl;
}
