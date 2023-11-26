#pragma once
#include <fstream>
#include <vector>
#include <vulkan/vulkan.hpp>

class VulkanShaderModule
{
public:
	VulkanShaderModule(vk::Device logicalDevice, const std::string& filePath);
	~VulkanShaderModule();

	vk::ShaderModule& GetShaderModule();

private:
	vk::Device m_logicalDevice;
	vk::ShaderModule m_shaderModule;

	std::vector<char> ReadFile(const std::string& filePath);
};