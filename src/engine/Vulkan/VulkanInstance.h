#pragma once
#include <vulkan/vulkan.hpp>

class VulkanInstance
{
public:
	VulkanInstance(const char* appName, const char* engineName);
	~VulkanInstance();

	vk::Instance* GetInstance();

private:
	vk::UniqueInstance instance;

	// Debug stuff
	vk::UniqueHandle<vk::DebugUtilsMessengerEXT, vk::DispatchLoaderDynamic> m_debugMessenger;
	vk::DispatchLoaderDynamic m_dispatchLoader;

	void SetupDebugMessenger();
	void PopulateDebugMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT& createInfo);
};
