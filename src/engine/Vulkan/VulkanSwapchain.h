#pragma once
#include <vulkan/vulkan.hpp>

class VulkanSwapchain
{
public:
	VulkanSwapchain(vk::Device& logicalDevice);
	~VulkanSwapchain();

	void CreateSwapchain();
	void DestroySwapchain();

private:
	vk::SwapchainKHR swapchain;
};
