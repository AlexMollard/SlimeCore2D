#include "VulkanRenderPass.h"

VulkanRenderPass::VulkanRenderPass(VulkanDevice& device, vk::SurfaceKHR surface) : m_device(device)
{
	CreateRenderPass(device.GetPhysicalDevice(), surface);
}

VulkanRenderPass::~VulkanRenderPass()
{
	DestroyRenderPass();
}

vk::SurfaceFormatKHR VulkanRenderPass::ChooseSurfaceFormat(vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface)
{
	auto surfaceFormats = physicalDevice.getSurfaceFormatsKHR(surface);

	for (const auto& format : surfaceFormats)
	{
		// Choose a suitable format based on your requirements
		if (format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
		{
			return format;
		}
	}

	// If the preferred format is not available, return the first available format
	return surfaceFormats[0];
}

vk::SurfaceFormatKHR VulkanRenderPass::GetSurfaceFormat() const 
{
	return m_surfaceFormat;
}

void VulkanRenderPass::CreateRenderPass(vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface)
{
	m_surfaceFormat = ChooseSurfaceFormat(physicalDevice, surface);

	vk::AttachmentDescription colorAttachment({}, m_surfaceFormat.format, vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore,
	                                          vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined, vk::ImageLayout::ePresentSrcKHR);

	vk::AttachmentReference colorAttachmentRef(0, vk::ImageLayout::eColorAttachmentOptimal);

	vk::SubpassDescription subpass({}, vk::PipelineBindPoint::eGraphics, 0, nullptr, 1, &colorAttachmentRef);

	vk::SubpassDependency dependency(VK_SUBPASS_EXTERNAL, 0, vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eColorAttachmentOutput,
	                                 vk::AccessFlags(), vk::AccessFlagBits::eColorAttachmentWrite);

	vk::RenderPassCreateInfo renderPassInfo({}, 1, &colorAttachment, 1, &subpass, 1, &dependency);
	m_renderPass = m_device.GetLogicalDevice().createRenderPass(renderPassInfo);
}

void VulkanRenderPass::DestroyRenderPass()
{
	if (m_renderPass)
	{
		m_device.GetLogicalDevice().destroyRenderPass(m_renderPass);
		m_renderPass = nullptr;
	}
}

vk::RenderPass VulkanRenderPass::GetRenderPass() const
{
	return m_renderPass;
}