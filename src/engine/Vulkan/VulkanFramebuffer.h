#pragma once
#include <vulkan/vulkan.hpp>

class VulkanFramebuffer
{
public:
	VulkanFramebuffer(vk::Device& logicalDevice, vk::RenderPass& renderPass, vk::Extent2D& extent);
	~VulkanFramebuffer();

	void CreateFramebuffer();
	void DestroyFramebuffer();

private:
	vk::Device m_logicalDevice;
	vk::RenderPass m_renderPass;
	vk::Extent2D m_extent;
	vk::UniqueFramebuffer m_framebuffer;
};
