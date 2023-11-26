#pragma once
#include <vulkan/vulkan.hpp>
#include "VulkanShaderModule.h"
#include "VulkanRenderPass.h"
#include "VulkanDevice.h"

class VulkanPipeline
{
public:
	VulkanPipeline(VulkanDevice& device, VulkanRenderPass& renderPass, vk::SurfaceKHR surface);
	~VulkanPipeline();

private:
	VulkanDevice& m_device;
	vk::Pipeline m_graphicsPipeline = nullptr;
	VulkanRenderPass& m_renderPass;

	VulkanShaderModule* vertShaderModule = nullptr;
	VulkanShaderModule* fragShaderModule = nullptr;

	void SetupViewportAndScissor(vk::SurfaceKHR surface, vk::Viewport& viewport, vk::Rect2D& scissor);
	vk::DescriptorSetLayout CreateDescriptorSetLayout();
};
