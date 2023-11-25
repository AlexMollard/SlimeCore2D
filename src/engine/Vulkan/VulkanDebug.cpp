#include "VulkanDebug.h"
#include "engine/ConsoleLog.h"

std::unordered_map<VkInstance, PFN_vkCreateDebugUtilsMessengerEXT> VulkanDebug::CreateDebugUtilsMessengerEXTDispatchTable;
std::unordered_map<VkInstance, PFN_vkDestroyDebugUtilsMessengerEXT> VulkanDebug::DestroyDebugUtilsMessengerEXTDispatchTable;
std::unordered_map<VkInstance, PFN_vkSubmitDebugUtilsMessageEXT> VulkanDebug::SubmitDebugUtilsMessageEXTDispatchTable;

void VulkanDebug::loadDebugUtilsCommands(VkInstance instance)
{
	PFN_vkVoidFunction temp_fp;

	temp_fp = vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (!temp_fp)
		SLIME_ERROR("Failed to load vkCreateDebugUtilsMessengerEXT");
	CreateDebugUtilsMessengerEXTDispatchTable[instance] = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(temp_fp);

	temp_fp = vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (!temp_fp)
		SLIME_ERROR("Failed to load vkDestroyDebugUtilsMessengerEXT");
	DestroyDebugUtilsMessengerEXTDispatchTable[instance] = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(temp_fp);

	temp_fp = vkGetInstanceProcAddr(instance, "vkSubmitDebugUtilsMessageEXT");
	if (!temp_fp)
		SLIME_ERROR("Failed to load vkSubmitDebugUtilsMessageEXT");
	SubmitDebugUtilsMessageEXTDispatchTable[instance] = reinterpret_cast<PFN_vkSubmitDebugUtilsMessageEXT>(temp_fp);
}

void VulkanDebug::unloadDebugUtilsCommands(VkInstance instance)
{
	CreateDebugUtilsMessengerEXTDispatchTable.erase(instance);
	DestroyDebugUtilsMessengerEXTDispatchTable.erase(instance);
	SubmitDebugUtilsMessageEXTDispatchTable.erase(instance);
}

VKAPI_ATTR VkResult VKAPI_CALL VulkanDebug::vkCreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                                                           const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pMessenger)
{
	auto dispatched_cmd = CreateDebugUtilsMessengerEXTDispatchTable.at(instance);
	return dispatched_cmd(instance, pCreateInfo, pAllocator, pMessenger);
}

VKAPI_ATTR void VKAPI_CALL VulkanDebug::vkDestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT messenger, const VkAllocationCallbacks* pAllocator)
{
	auto dispatched_cmd = DestroyDebugUtilsMessengerEXTDispatchTable.at(instance);
	return dispatched_cmd(instance, messenger, pAllocator);
}

VKAPI_ATTR void VKAPI_CALL VulkanDebug::vkSubmitDebugUtilsMessageEXT(VkInstance instance, VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                                     VkDebugUtilsMessageTypeFlagsEXT messageTypes, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData)
{
	auto dispatched_cmd = SubmitDebugUtilsMessageEXTDispatchTable.at(instance);
	return dispatched_cmd(instance, messageSeverity, messageTypes, pCallbackData);
}

VkBool32 VulkanDebug::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
                                    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
	// Your callback implementation
	if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
	{
		SLIME_WARN("Validation layer: {}", pCallbackData->pMessage);
	}

	// Return VK_FALSE to continue the normal execution of the Vulkan function that triggered the validation layer message.
	return VK_FALSE;
}