#pragma once
#include <vulkan/vulkan.hpp>

class VulkanShaderModule
{
public:
	VulkanShaderModule(vk::Device* logicalDevice, const std::string& filePath);
	~VulkanShaderModule();

	vk::ShaderModule GetShaderModule() const;

private:
	vk::Device* m_logicalDevice = nullptr;
	vk::UniqueShaderModule m_shaderModule;
};
