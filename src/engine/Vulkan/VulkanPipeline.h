#pragma once
#include <vulkan/vulkan.hpp>
#include "VulkanShaderModule.h"
#include "VulkanRenderPass.h"

class VulkanPipeline
{
public:
	VulkanPipeline(vk::Device logicalDevice, VulkanRenderPass renderPass);
	~VulkanPipeline();

	void CreateGraphicsPipeline(const VulkanShaderModule& vertexShader, const VulkanShaderModule& fragmentShader);
	void DestroyGraphicsPipeline();

private:
	vk::Device m_logicalDevice;
	vk::UniquePipeline m_graphicsPipeline;
	VulkanRenderPass m_renderPass;
};
