#include "VulkanShaderModule.h"

VulkanShaderModule::VulkanShaderModule(vk::Device* logicalDevice, const std::string& filePath) {}

VulkanShaderModule::~VulkanShaderModule() {}

vk::ShaderModule VulkanShaderModule::GetShaderModule() const 
{
	return vk::ShaderModule();
}
