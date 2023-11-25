#pragma once
#include <unordered_map>
#include <vulkan/vulkan.hpp>

class VulkanDebug
{
public:
	static std::unordered_map<VkInstance, PFN_vkCreateDebugUtilsMessengerEXT> CreateDebugUtilsMessengerEXTDispatchTable;
	static std::unordered_map<VkInstance, PFN_vkDestroyDebugUtilsMessengerEXT> DestroyDebugUtilsMessengerEXTDispatchTable;
	static std::unordered_map<VkInstance, PFN_vkSubmitDebugUtilsMessageEXT> SubmitDebugUtilsMessageEXTDispatchTable;

	static void loadDebugUtilsCommands(VkInstance instance);
	static void unloadDebugUtilsCommands(VkInstance instance);

	static VKAPI_ATTR VkResult VKAPI_CALL vkCreateDebugUtilsMessengerEXT(VkInstance instance,
	                                                                     const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
	                                                                     const VkAllocationCallbacks* pAllocator,
	                                                                     VkDebugUtilsMessengerEXT* pMessenger);

	static VKAPI_ATTR void VKAPI_CALL vkDestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT messenger, const VkAllocationCallbacks* pAllocator);

	static VKAPI_ATTR void VKAPI_CALL vkSubmitDebugUtilsMessageEXT(VkInstance instance,
	                                                               VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	                                                               VkDebugUtilsMessageTypeFlagsEXT messageTypes,
	                                                               const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData);
	// Debug callback
	static VkBool32 DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	                              VkDebugUtilsMessageTypeFlagsEXT messageType,
	                              const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	                              void* pUserData);
};
