#include "VulkanDevice.h"

#include <engine/ConsoleLog.h>
#include <set>

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
