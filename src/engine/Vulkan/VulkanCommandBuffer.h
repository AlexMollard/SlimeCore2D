#pragma once
#include <vulkan/vulkan.hpp>

class VulkanCommandBuffer
{
public:
	VulkanCommandBuffer(vk::Device logicalDevice);
	~VulkanCommandBuffer();

    vk::CommandBuffer BeginCommandBuffer(vk::CommandPool& commandPool, vk::CommandBufferUsageFlags usageFlags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
	void EndCommandBuffer(vk::CommandPool& commandPool, vk::Queue queue, vk::CommandBuffer commandBuffer);

private:
	vk::Device m_logicalDevice;
};
