#pragma once
#include <vulkan/vulkan.hpp>
#include "VulkanShaderModule.h"

class VulkanPipeline
{
public:
	VulkanPipeline(vk::Device& logicalDevice, vk::RenderPass& renderPass);
	~VulkanPipeline();

	void CreateGraphicsPipeline(const VulkanShaderModule& vertexShader, const VulkanShaderModule& fragmentShader);
	void DestroyGraphicsPipeline();

private:
	vk::Device m_logicalDevice;
	vk::UniquePipeline m_graphicsPipeline;
	vk::RenderPass m_renderPass;
};
