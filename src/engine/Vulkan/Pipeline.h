#pragma once

#include <vulkan/vulkan.h>

namespace vkutil
{
	bool LoadShaderModule(const char* filePath, VkDevice device, VkShaderModule* outShaderModule);
} // namespace vkutil