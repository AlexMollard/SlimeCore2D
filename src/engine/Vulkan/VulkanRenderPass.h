#pragma once
#include "VulkanDevice.h"

class VulkanRenderPass
{
public:
	VulkanRenderPass(VulkanDevice& device, vk::SurfaceKHR surface);
	~VulkanRenderPass();

	vk::SurfaceFormatKHR ChooseSurfaceFormat(vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface);
	vk::SurfaceFormatKHR GetSurfaceFormat() const;
	
	vk::RenderPass GetRenderPass() const;

private:
	VulkanDevice& m_device;
	vk::SurfaceFormatKHR m_surfaceFormat;
	vk::RenderPass m_renderPass = nullptr;

	void PrintDebugInfo(const vk::SurfaceKHR& surface);
	void AddValueToBuffer(const char* name, const auto& value);
};
