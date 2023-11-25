#include "VulkanInstance.h"
#include "VulkanDebug.h"

VulkanInstance::VulkanInstance(const char* appName, const char* engineName)
{
	vk::ApplicationInfo appInfo(appName, VK_MAKE_VERSION(1, 0, 0), engineName, VK_MAKE_VERSION(1, 0, 0), VK_API_VERSION_1_0);

	// Specify the required extensions
	const char* extensions[] = {
		VK_KHR_SURFACE_EXTENSION_NAME, VK_KHR_WIN32_SURFACE_EXTENSION_NAME, VK_EXT_DEBUG_UTILS_EXTENSION_NAME
	};

	vk::InstanceCreateInfo createInfo({}, &appInfo);
	createInfo.enabledExtensionCount   = sizeof(extensions) / sizeof(extensions[0]);
	createInfo.ppEnabledExtensionNames = extensions;

	instance = vk::createInstanceUnique(createInfo);

	VulkanDebug::loadDebugUtilsCommands(instance.get());
	SetupDebugMessenger();
}

VulkanInstance::~VulkanInstance() 
{
	VulkanDebug::unloadDebugUtilsCommands(instance.get());
	instance.release();
}

vk::Instance* VulkanInstance::GetInstance()
{
	return &instance.get();
}

void VulkanInstance::SetupDebugMessenger() 
{
	vk::DebugUtilsMessengerCreateInfoEXT createInfo(
	    {}, vk::DebugUtilsMessageSeverityFlagBitsEXT::eError | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose,
	    vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
	    VulkanDebug::DebugCallback, nullptr);

	PopulateDebugMessengerCreateInfo(createInfo);

	m_dispatchLoader.init(instance.get(), vkGetInstanceProcAddr);
	m_debugMessenger = instance.get().createDebugUtilsMessengerEXTUnique(createInfo, nullptr, m_dispatchLoader);
}

void VulkanInstance::PopulateDebugMessengerCreateInfo(vk::DebugUtilsMessengerCreateInfoEXT& createInfo) 
{
	createInfo.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eError | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose;
	createInfo.messageType     = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;
	createInfo.pfnUserCallback = VulkanDebug::DebugCallback;
	createInfo.pUserData       = nullptr;
}
