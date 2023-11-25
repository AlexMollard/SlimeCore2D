#pragma once
#include <optional>
#include <vulkan/vulkan.hpp>

class VulkanDevice
{
public:
	enum class QueueType
	{
		GRAPHICS,
		PRESENT,
		COMPUTE,
		TRANSFER
	};

	VulkanDevice(vk::Instance& instance, vk::SurfaceKHR& surface);
	~VulkanDevice();

	vk::PhysicalDevice* GetPhysicalDevice();
	vk::Device& GetLogicalDevice();

	// == QUEUE == //
	vk::Queue GetQueue(QueueType queueType) const;
	uint32_t GetQueueIndex(QueueType queueType) const;
	uint32_t GetQueueFamilyIndex(QueueType queueType) const;

private:
	void SetQueue(QueueType queueType, vk::Queue queue);
	void SetQueueIndex(QueueType queueType, uint32_t queueIndex);
	void SetQueueFamilyIndex(QueueType queueType, uint32_t queueFamilyIndex);

	bool IsQueueFamilyIndexSet(QueueType queueType) const;

	vk::PhysicalDevice m_physicalDevice;
	vk::Device m_logicalDevice;

	vk::Queue m_graphicsQueue; // Used for pushing geometry to the GPU
	vk::Queue m_presentQueue;  // Used for presenting the rendered image to the screen
	vk::Queue m_computeQueue;  // Used for compute shaders
	vk::Queue m_transferQueue; // Used for transferring data between buffers

	uint32_t m_graphicsQueueIndex = UINT32_MAX;
	uint32_t m_presentQueueIndex  = UINT32_MAX;
	uint32_t m_computeQueueIndex  = UINT32_MAX;
	uint32_t m_transferQueueIndex = UINT32_MAX;

	uint32_t m_graphicsQueueFamilyIndex = UINT32_MAX;
	uint32_t m_presentQueueFamilyIndex  = UINT32_MAX;
	uint32_t m_computeQueueFamilyIndex  = UINT32_MAX;
	uint32_t m_transferQueueFamilyIndex = UINT32_MAX;

	struct QueueFamilyIndices
	{
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		bool IsComplete() const;
	};

	// == COMMAND POOL == //
	vk::CommandPool CreateCommandPool(QueueType queueType);

	vk::CommandPool m_graphicsCommandPool;
	vk::CommandPool m_presentCommandPool;

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
	std::vector<const char*> m_deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

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
