#pragma once
#include <vulkan/vulkan.hpp>
#include "VulkanDevice.h"
#include "VulkanRenderPass.h"

class VulkanFramebuffers;

class VulkanSwapchain
{
public:
	VulkanSwapchain(VulkanDevice& device, const vk::SurfaceKHR& surface);
	~VulkanSwapchain();

	void CreateSwapchain(const vk::SurfaceKHR& surface);
	void DestroySwapchain();
private:
	vk::SurfaceCapabilitiesKHR GetSurfaceCapabilities(const vk::SurfaceKHR& surface);
	vk::SurfaceFormatKHR ChooseSurfaceFormat(const vk::SurfaceKHR& surface);
	vk::PresentModeKHR ChoosePresentMode(const vk::SurfaceKHR& surface);

	VulkanDevice& m_device;
	vk::SwapchainKHR m_swapchain;
	std::vector<vk::Image> m_swapchainImages;
	VulkanFramebuffers* m_framebuffers = nullptr;
	vk::Extent2D m_swapchainExtent;
	VulkanRenderPass m_renderPass;

	// Debug stuff
	void PrintDebugInfo(const vk::SurfaceKHR& surface);
	void AddValueToBuffer(const char* name, const auto& value);
	void PrintSurfaceCapabilities(const vk::SurfaceCapabilitiesKHR& surfaceCapabilities);
	void PrintSurfaceFormats(const std::vector<vk::SurfaceFormatKHR>& surfaceFormats);
	void PrintPresentModes(const std::vector<vk::PresentModeKHR>& presentModes);
};
