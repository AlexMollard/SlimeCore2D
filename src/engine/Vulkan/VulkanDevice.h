#pragma once
#include <vulkan/vulkan.hpp>
#include <optional>

class VulkanDevice
{
public:
	VulkanDevice(vk::Instance& instance, vk::SurfaceKHR& surface);
	~VulkanDevice();

	vk::PhysicalDevice* GetPhysicalDevice();
	vk::Device* GetLogicalDevice();

private:
	vk::PhysicalDevice m_physicalDevice;
	vk::UniqueDevice m_logicalDevice;

	struct QueueFamilyIndices
	{
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		bool IsComplete() const
		{
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};

	struct SwapChainSupportDetails
	{
		vk::SurfaceCapabilitiesKHR capabilities;
		std::vector<vk::SurfaceFormatKHR> formats;
		std::vector<vk::PresentModeKHR> presentModes;
	};

	bool IsDeviceSuitable(vk::PhysicalDevice device, vk::SurfaceKHR surface);
	bool CheckDeviceExtensionSupport(vk::PhysicalDevice device);
	QueueFamilyIndices FindQueueFamilies(vk::PhysicalDevice device, vk::SurfaceKHR surface);
	SwapChainSupportDetails QuerySwapChainSupport(vk::PhysicalDevice device, vk::SurfaceKHR surface);

	// Device extensions
	const std::vector<const char*> m_deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};
};
