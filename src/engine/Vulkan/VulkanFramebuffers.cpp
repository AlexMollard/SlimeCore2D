#include "VulkanFramebuffers.h"

VulkanFramebuffers::VulkanFramebuffers(vk::Device logicalDevice, VulkanRenderPass& renderPass, vk::Extent2D& extent, const std::vector<vk::Image>& swapchainImages)
    : m_logicalDevice(logicalDevice), m_renderPass(renderPass), m_extent(extent)
{
	m_framebuffers.reserve(swapchainImages.size());

	for (const auto& swapchainImage : swapchainImages)
	{
		// Create an image view for each swapchain image
		vk::ImageViewCreateInfo imageViewInfo({}, swapchainImage, vk::ImageViewType::e2D, renderPass.GetSurfaceFormat().format, {}, { vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 });
		vk::ImageView imageView = m_logicalDevice.createImageView(imageViewInfo);

		// Create a framebuffer for each image view
		vk::FramebufferCreateInfo framebufferInfo({}, m_renderPass.GetRenderPass(), 1, &imageView, m_extent.width, m_extent.height, 1);
		vk::Framebuffer framebuffer = m_logicalDevice.createFramebuffer(framebufferInfo);

		m_imageViews.push_back(imageView);
		m_framebuffers.push_back(framebuffer);
	}
}

VulkanFramebuffers::~VulkanFramebuffers()
{
	for (const auto& framebuffer : m_framebuffers)
	{
		m_logicalDevice.destroyFramebuffer(framebuffer);
	}

	for (const auto& imageView : m_imageViews)
	{
		m_logicalDevice.destroyImageView(imageView);
	}

	m_framebuffers.clear();
	m_imageViews.clear();
}
