#pragma once
#include <vulkan/vulkan.hpp>
#include "VulkanRenderPass.h"

class VulkanFramebuffers
{
public:
	VulkanFramebuffers(vk::Device logicalDevice, VulkanRenderPass& renderPass, vk::Extent2D& extent, const std::vector<vk::Image>& swapchainImages);
	~VulkanFramebuffers();

private:
	vk::Device m_logicalDevice;
	VulkanRenderPass& m_renderPass;
	vk::Extent2D m_extent;
	std::vector<vk::ImageView> m_imageViews;
	std::vector<vk::Framebuffer> m_framebuffers;
};
