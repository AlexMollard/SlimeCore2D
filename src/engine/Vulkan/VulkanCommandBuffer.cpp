#include "VulkanCommandBuffer.h"

VulkanCommandBuffer::VulkanCommandBuffer(vk::Device logicalDevice) {}

VulkanCommandBuffer::~VulkanCommandBuffer() {}

vk::CommandBuffer VulkanCommandBuffer::BeginSingleTimeCommands() 
{
	return vk::CommandBuffer();
}

void VulkanCommandBuffer::EndSingleTimeCommands(vk::CommandBuffer& commandBuffer) {}
