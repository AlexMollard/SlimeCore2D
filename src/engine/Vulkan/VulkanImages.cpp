#include "VulkanImages.h"
#include "VulkanInit.h"

void vkutil::TransitionImage(VkCommandBuffer cmd, VkImage image, VkImageLayout currentLayout, VkImageLayout newLayout) 
{
	VkImageMemoryBarrier2 imageBarrier{};
	imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
	imageBarrier.pNext = nullptr;

	imageBarrier.srcStageMask  = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
	imageBarrier.srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT;
	imageBarrier.dstStageMask  = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
	imageBarrier.dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT;

	imageBarrier.oldLayout = currentLayout;
	imageBarrier.newLayout = newLayout;

	VkImageAspectFlags aspectMask = (newLayout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT;
	imageBarrier.subresourceRange = vkinit::ImageSubresourceRange(aspectMask);
	imageBarrier.image            = image;

	VkDependencyInfo depInfo{};
	depInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
	depInfo.pNext = nullptr;

	depInfo.imageMemoryBarrierCount = 1;
	depInfo.pImageMemoryBarriers    = &imageBarrier;

	vkCmdPipelineBarrier2(cmd, &depInfo);
}

void vkutil::CopyImageToImage(VkCommandBuffer cmd, VkImage source, VkImage destination, VkExtent3D imageSize)
{
	VkImageBlit2 blitRegion{};
	blitRegion.sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2;
	blitRegion.pNext = nullptr;

	blitRegion.srcOffsets[1].x = imageSize.width;
	blitRegion.srcOffsets[1].y = imageSize.height;
	blitRegion.srcOffsets[1].z = 1;

	blitRegion.dstOffsets[1].x = imageSize.width;
	blitRegion.dstOffsets[1].y = imageSize.height;
	blitRegion.dstOffsets[1].z = 1;

	blitRegion.srcSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
	blitRegion.srcSubresource.baseArrayLayer = 0;
	blitRegion.srcSubresource.layerCount     = 1;
	blitRegion.srcSubresource.mipLevel       = 0;

	blitRegion.dstSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
	blitRegion.dstSubresource.baseArrayLayer = 0;
	blitRegion.dstSubresource.layerCount     = 1;
	blitRegion.dstSubresource.mipLevel       = 0;

	VkBlitImageInfo2 blitInfo{};
	blitInfo.sType          = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2;
	blitInfo.pNext          = nullptr;
	blitInfo.dstImage       = destination;
	blitInfo.dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
	blitInfo.srcImage       = source;
	blitInfo.srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
	blitInfo.filter         = VK_FILTER_NEAREST;
	blitInfo.regionCount    = 1;
	blitInfo.pRegions       = &blitRegion;

	vkCmdBlitImage2(cmd, &blitInfo);
}
