#include "VulkanCommandBuffer.h"
#include <limits>

VulkanCommandBuffer::VulkanCommandBuffer(vk::Device logicalDevice) : m_logicalDevice(logicalDevice) {}

VulkanCommandBuffer::~VulkanCommandBuffer() {}

vk::CommandBuffer VulkanCommandBuffer::BeginCommandBuffer(vk::CommandPool& commandPool, vk::CommandBufferUsageFlags usageFlags)
{
	vk::CommandBufferAllocateInfo allocInfo;
	allocInfo.setCommandPool(commandPool);
	allocInfo.setLevel(vk::CommandBufferLevel::ePrimary);
	allocInfo.setCommandBufferCount(1);

	vk::CommandBuffer commandBuffer = m_logicalDevice.allocateCommandBuffers(allocInfo)[0];

	vk::CommandBufferBeginInfo beginInfo;
	beginInfo.setFlags(usageFlags);
	
	commandBuffer.begin(beginInfo);

	return commandBuffer;
}

void VulkanCommandBuffer::EndCommandBuffer(vk::CommandPool& commandPool, vk::Queue queue, vk::CommandBuffer commandBuffer)
{
	commandBuffer.end();

	vk::SubmitInfo submitInfo;
	submitInfo.setCommandBufferCount(1);
	submitInfo.setPCommandBuffers(&commandBuffer);

	queue.submit(submitInfo, nullptr);
	queue.waitIdle();

	m_logicalDevice.freeCommandBuffers(commandPool, commandBuffer);
}