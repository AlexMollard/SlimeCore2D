#include "VulkanDevice.h"

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
	float queuePriority = 1.0f;
	vk::DeviceQueueCreateInfo queueCreateInfo({}, m_physicalDevice.getQueueFamilyProperties()[0].queueCount);

	vk::DeviceCreateInfo deviceCreateInfo({}, 1, &queueCreateInfo);

	m_logicalDevice = m_physicalDevice.createDeviceUnique(deviceCreateInfo);

	OutputDeviceProperties();
}

VulkanDevice::~VulkanDevice()
{
	m_logicalDevice->waitIdle();
	m_logicalDevice.reset();
}

vk::PhysicalDevice* VulkanDevice::GetPhysicalDevice()
{
	return &m_physicalDevice;
}

vk::Device* VulkanDevice::GetLogicalDevice()
{
	return &m_logicalDevice.get();
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

std::string VulkanDevice::GetFormattedApiVersion(uint32_t apiVersion)
{
	return std::to_string(VK_VERSION_MAJOR(apiVersion)) + "." + std::to_string(VK_VERSION_MINOR(apiVersion)) + "." + std::to_string(VK_VERSION_PATCH(apiVersion));
}

void VulkanDevice::AddValueToBuffer(const char* name, auto value)
{
	std::cout << std::setw(30) << std::left << name << ": " << value << "\n";
}

void VulkanDevice::OutputDeviceProperties()
{
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

	std::cout << std::flush	<< std::endl;
}
