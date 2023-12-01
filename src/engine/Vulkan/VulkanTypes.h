#pragma once
#include "engine/ConsoleLog.h"
#include "vulkan/vk_mem_alloc.h"
#include <vulkan/vulkan.h>
#include "glm.hpp"

struct AllocatedImage
{
	VkFormat imageFormat;
	VkImage image;
	VkImageView imageView;
	VmaAllocation allocation;

	void Destroy(VkDevice device, VmaAllocator allocator);
};

struct AllocatedBuffer
{
	VkBuffer buffer;
	VmaAllocation allocation;
	VmaAllocationInfo info;
};

struct Vertex
{
	glm::vec3 position;
	float uv_x;
	glm::vec3 normal;
	float uv_y;
	glm::vec4 color;
};

struct GPUMeshBuffers
{
	AllocatedBuffer indexBuffer;
	AllocatedBuffer vertexBuffer;
	VkDeviceAddress vertexBufferAddress;
};

struct VulkanBuffer
{
	VulkanBuffer();

	// Setters
	void SetBufferHandle(VkBuffer handle);
	void SetAllocation(VmaAllocation alloc);
	void SetSize(size_t bufferSize);

	// Getters
	VkBuffer GetBufferHandle() const;
	VmaAllocation GetAllocation() const;
	size_t GetSize() const;

private:
	VkBuffer bufferHandle;
	VmaAllocation allocation;
	size_t size;
};

// DEGUB STUFF BELOW
static const char* vkResultToString(VkResult res)
{
	switch (res)
	{
#define CASE(x)                                                                                                                                                               \
	case VK_##x: return #x;
		CASE(SUCCESS)
		CASE(NOT_READY)
		CASE(TIMEOUT)
		CASE(EVENT_SET)
		CASE(EVENT_RESET)
		CASE(INCOMPLETE)
		CASE(ERROR_OUT_OF_HOST_MEMORY)
		CASE(ERROR_OUT_OF_DEVICE_MEMORY)
		CASE(ERROR_INITIALIZATION_FAILED)
		CASE(ERROR_DEVICE_LOST)
		CASE(ERROR_MEMORY_MAP_FAILED)
		CASE(ERROR_LAYER_NOT_PRESENT)
		CASE(ERROR_EXTENSION_NOT_PRESENT)
		CASE(ERROR_FEATURE_NOT_PRESENT)
		CASE(ERROR_INCOMPATIBLE_DRIVER)
		CASE(ERROR_TOO_MANY_OBJECTS)
		CASE(ERROR_FORMAT_NOT_SUPPORTED)
		CASE(ERROR_FRAGMENTED_POOL)
		CASE(ERROR_UNKNOWN)
		CASE(ERROR_OUT_OF_POOL_MEMORY)
		CASE(ERROR_INVALID_EXTERNAL_HANDLE)
		CASE(ERROR_FRAGMENTATION)
		CASE(ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS)
		CASE(PIPELINE_COMPILE_REQUIRED)
		CASE(ERROR_SURFACE_LOST_KHR)
		CASE(ERROR_NATIVE_WINDOW_IN_USE_KHR)
		CASE(SUBOPTIMAL_KHR)
		CASE(ERROR_OUT_OF_DATE_KHR)
		CASE(ERROR_INCOMPATIBLE_DISPLAY_KHR)
		CASE(ERROR_VALIDATION_FAILED_EXT)
		CASE(ERROR_INVALID_SHADER_NV)
#ifdef VK_ENABLE_BETA_EXTENSIONS
		CASE(ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR)
		CASE(ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR)
		CASE(ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR)
		CASE(ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR)
		CASE(ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR)
		CASE(ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR)
#endif
		CASE(ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT)
		CASE(ERROR_NOT_PERMITTED_KHR)
		CASE(ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT)
		CASE(THREAD_IDLE_KHR)
		CASE(THREAD_DONE_KHR)
		CASE(OPERATION_DEFERRED_KHR)
		CASE(OPERATION_NOT_DEFERRED_KHR)
	default: return "unknown";
	}
#undef CASE
}

#define VK_CHECK(x)                                                                                                                                                           \
	do                                                                                                                                                                        \
	{                                                                                                                                                                         \
		VkResult err = x;                                                                                                                                                     \
		if (err != VK_SUCCESS)                                                                                                                                                              \
		{                                                                                                                                                                     \
			SLIME_ERROR("Detected Vulkan error: %s", vkResultToString(err));                                                                                                  \
		}                                                                                                                                                                     \
	}                                                                                                                                                                         \
	while (0)