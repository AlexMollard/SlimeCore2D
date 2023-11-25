#pragma once
#include <optional>
#include <vulkan/vulkan.hpp>

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

		bool IsComplete() const;
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
	const std::vector<const char*> m_deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	// Debug output stuff
	enum class VendorID
	{
		Unknown  = 0,
		AMD      = 0x1002,
		ImgTec   = 0x1010,
		NVIDIA   = 0x10DE,
		ARM      = 0x13B5,
		Qualcomm = 0x5143,
		INTEL    = 0x8086
	};

	const char* BoolToString(bool value);
	const char* GetVendorName(uint32_t vendorID);
	const char* GetDeviceTypeName(vk::PhysicalDeviceType deviceType);
	std::string GetFormattedApiVersion(uint32_t apiVersion);
	void AddValueToBuffer(const char* name, auto value);

	void OutputDeviceProperties();
};
