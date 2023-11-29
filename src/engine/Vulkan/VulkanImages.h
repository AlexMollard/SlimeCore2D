#pragma once
#include <vulkan/vulkan.h>

namespace vkutil
{
void TransitionImage(VkCommandBuffer cmd, VkImage image, VkImageLayout currentLayout, VkImageLayout newLayout);
void CopyImageToImage(VkCommandBuffer cmd, VkImage source, VkImage destination, VkExtent3D imageSize);
}