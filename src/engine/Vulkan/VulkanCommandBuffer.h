#pragma once
#include <vulkan/vulkan.hpp>

class VulkanCommandBuffer
{
public:
	VulkanCommandBuffer(vk::Device logicalDevice);
	~VulkanCommandBuffer();

	vk::CommandBuffer BeginSingleTimeCommands();
	void EndSingleTimeCommands(vk::CommandBuffer& commandBuffer);

private:
	vk::Device m_logicalDevice;
};
