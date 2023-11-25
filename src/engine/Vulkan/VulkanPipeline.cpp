#include "VulkanPipeline.h"

VulkanPipeline::VulkanPipeline(vk::Device logicalDevice, VulkanRenderPass renderPass) : m_logicalDevice(logicalDevice), m_renderPass(renderPass) {}

VulkanPipeline::~VulkanPipeline() {}

void VulkanPipeline::CreateGraphicsPipeline(const VulkanShaderModule& vertexShader, const VulkanShaderModule& fragmentShader) {}

void VulkanPipeline::DestroyGraphicsPipeline() {}
