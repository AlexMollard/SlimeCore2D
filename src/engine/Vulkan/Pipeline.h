#pragma once

#include <vulkan/vulkan.h>

namespace vkutil
{
	VkShaderModule LoadShaderModule(const char* filePath, VkDevice device);
} // namespace vkutil