#include "VulkanShaderModule.h"
#include <iostream>

VulkanShaderModule::VulkanShaderModule(vk::Device logicalDevice, const std::string& filePath) : m_logicalDevice(logicalDevice), m_shaderModule(nullptr)
{
	// Read the shader source code from the file
	auto code = ReadFile(filePath);

	// Create a Vulkan shader module
	vk::ShaderModuleCreateInfo createInfo({}, code.size(), reinterpret_cast<const uint32_t*>(code.data()));
	m_shaderModule = m_logicalDevice.createShaderModule(createInfo);
}

VulkanShaderModule::~VulkanShaderModule()
{
	if (m_shaderModule)
	{
		m_logicalDevice.destroyShaderModule(m_shaderModule);
	}
}

vk::ShaderModule& VulkanShaderModule::GetShaderModule()
{
	return m_shaderModule;
}

std::vector<char> VulkanShaderModule::ReadFile(const std::string& filePath)
{
	// Read the file in binary mode
	std::ifstream file(filePath, std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		// Handle file not found or other errors
		throw std::runtime_error("Failed to open file: " + filePath);
	}

	size_t fileSize = static_cast<size_t>(file.tellg());
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;
}