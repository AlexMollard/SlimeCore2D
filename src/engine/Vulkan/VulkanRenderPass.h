#pragma once
#include <vulkan/vulkan.hpp>

class VulkanRenderPass
{
public:
	VulkanRenderPass(vk::Device* logicalDevice);
	~VulkanRenderPass();

	void CreateRenderPass();
	void DestroyRenderPass();

private:
	vk::Device* m_logicalDevice = nullptr;
	vk::UniqueRenderPass m_renderPass;
};
