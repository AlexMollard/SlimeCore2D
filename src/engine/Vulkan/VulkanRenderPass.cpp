#include "VulkanRenderPass.h"

#include <iomanip>
#include <iostream>

VulkanRenderPass::VulkanRenderPass(VulkanDevice& device, vk::SurfaceKHR surface) : m_device(device)
{
	m_surfaceFormat = ChooseSurfaceFormat(device.GetPhysicalDevice(), surface);

	vk::AttachmentDescription colorAttachment({}, m_surfaceFormat.format, vk::SampleCountFlagBits::e1, vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore,
	                                          vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare, vk::ImageLayout::eUndefined, vk::ImageLayout::ePresentSrcKHR);

	vk::AttachmentReference colorAttachmentRef(0, vk::ImageLayout::eColorAttachmentOptimal);

	vk::SubpassDescription subpass({}, vk::PipelineBindPoint::eGraphics, 0, nullptr, 1, &colorAttachmentRef);

	vk::SubpassDependency dependency(VK_SUBPASS_EXTERNAL, 0, vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eColorAttachmentOutput,
	                                 vk::AccessFlags(), vk::AccessFlagBits::eColorAttachmentWrite);

	vk::RenderPassCreateInfo renderPassInfo({}, 1, &colorAttachment, 1, &subpass, 1, &dependency);
	m_renderPass = m_device.GetLogicalDevice().createRenderPass(renderPassInfo);


	PrintDebugInfo(surface);
}

VulkanRenderPass::~VulkanRenderPass()
{
	if (m_renderPass)
	{
		m_device.GetLogicalDevice().destroyRenderPass(m_renderPass);
		m_renderPass = nullptr;
	}
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

vk::RenderPass VulkanRenderPass::GetRenderPass() const
{
	return m_renderPass;
}

void VulkanRenderPass::AddValueToBuffer(const char* name, const auto& value)
{
	std::cout << std::setw(20) << std::left << name << ": " << value << "\n";
}

void VulkanRenderPass::PrintDebugInfo(const vk::SurfaceKHR& surface)
{
	std::cout << "Render Pass Created:\n";
	AddValueToBuffer("  Surface format", vk::to_string(m_surfaceFormat.format));
	AddValueToBuffer("  Surface color space", vk::to_string(m_surfaceFormat.colorSpace));
	std::cout << std::endl;
}
