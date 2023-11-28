#include "VulkanInit.h"



VkCommandPoolCreateInfo vkinit::CommandPoolCreateInfo(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags flags /*= 0*/)
{
	VkCommandPoolCreateInfo info = {};
	info.sType                   = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	info.pNext                   = nullptr;

	info.flags = flags;
	return info;
}

VkCommandBufferAllocateInfo vkinit::CommandBufferAllocateInfo(VkCommandPool pool, uint32_t count /*= 1*/)
{
	VkCommandBufferAllocateInfo info = {};
	info.sType                       = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	info.pNext                       = nullptr;

	info.commandPool        = pool;
	info.commandBufferCount = count;
	info.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	return info;
}

VkCommandBufferBeginInfo vkinit::CommandBufferBeginInfo(VkCommandBufferUsageFlags flags /*= 0*/)
{
	VkCommandBufferBeginInfo info = {};
	info.sType                    = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	info.pNext                    = nullptr;

	info.pInheritanceInfo = nullptr;
	info.flags            = flags;
	return info;
}

VkFenceCreateInfo vkinit::FenceCreateInfo(VkFenceCreateFlags flags /*= 0*/)
{
	VkFenceCreateInfo info = {};
	info.sType             = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	info.pNext             = nullptr;

	info.flags = flags;

	return info;
}

VkSemaphoreCreateInfo vkinit::SemaphoreCreateInfo(VkSemaphoreCreateFlags flags /*= 0*/)
{
	VkSemaphoreCreateInfo info = {};
	info.sType                 = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	info.pNext                 = nullptr;
	info.flags                 = flags;
	return info;
}

VkImageSubresourceRange vkinit::ImageSubresourceRange(VkImageAspectFlags aspectMask)
{
	VkImageSubresourceRange subImage{};
	subImage.aspectMask     = aspectMask;
	subImage.baseMipLevel   = 0;
	subImage.levelCount     = 1;
	subImage.baseArrayLayer = 0;
	subImage.layerCount     = 1;

	return subImage;
}

VkSemaphoreSubmitInfo vkinit::SemaphoreSubmitInfo(VkPipelineStageFlags2 stageMask, VkSemaphore semaphore)
{
	VkSemaphoreSubmitInfo submitInfo{};
	submitInfo.sType       = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
	submitInfo.pNext       = nullptr;
	submitInfo.semaphore   = semaphore;
	submitInfo.stageMask   = stageMask;
	submitInfo.deviceIndex = 0;
	submitInfo.value       = 1;

	return submitInfo;
}

VkCommandBufferSubmitInfo vkinit::CommandBufferSubmitInfo(VkCommandBuffer cmd)
{
	VkCommandBufferSubmitInfo info{};
	info.sType         = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
	info.pNext         = nullptr;
	info.commandBuffer = cmd;
	info.deviceMask    = 0;

	return info;
}

VkSubmitInfo2 vkinit::SubmitInfo(VkCommandBufferSubmitInfo* cmd, VkSemaphoreSubmitInfo* signalSemaphoreInfo, VkSemaphoreSubmitInfo* waitSemaphoreInfo)
{
	VkSubmitInfo2 info = {};
	info.sType         = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
	info.pNext         = nullptr;

	info.waitSemaphoreInfoCount = waitSemaphoreInfo == nullptr ? 0 : 1;
	info.pWaitSemaphoreInfos    = waitSemaphoreInfo;

	info.signalSemaphoreInfoCount = signalSemaphoreInfo == nullptr ? 0 : 1;
	info.pSignalSemaphoreInfos    = signalSemaphoreInfo;

	info.commandBufferInfoCount = 1;
	info.pCommandBufferInfos    = cmd;

	return info;
}
