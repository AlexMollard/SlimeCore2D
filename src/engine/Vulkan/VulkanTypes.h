#pragma once

#include <mat4x4.hpp>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <vec4.hpp>
#include <vector>
#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vk_mem_alloc.h>
#include <vulkan/vulkan.h>

#include "engine/ConsoleLog.h"

// we will add our main reusable types here
struct AllocatedImage
{
	VkImage image;
	VkImageView imageView;
	VmaAllocation allocation;
	VkExtent3D imageExtent;
	VkFormat imageFormat;
};

struct AllocatedBuffer
{
	VkBuffer buffer;
	VmaAllocation allocation;
	VmaAllocationInfo info;
};

struct GPUGLTFMaterial
{
	glm::vec4 colorFactors;
	glm::vec4 metal_rough_factors;
	glm::vec4 extra[14];
};

static_assert(sizeof(GPUGLTFMaterial) == 256);

struct GPUSceneData
{
	glm::mat4 view;
	glm::mat4 proj;
	glm::mat4 viewproj;
	glm::vec4 ambientColor;
	glm::vec4 sunlightDirection; // w for sun power
	glm::vec4 sunlightColor;
};

enum class MaterialPass : uint8_t
{
	MainColor,
	Transparent,
	Other
};

struct MaterialData
{
	VkPipeline pipeline;
	VkPipelineLayout layout;
	VkDescriptorSet materialSet;

	MaterialPass passType;
};

struct Vertex
{

	glm::vec3 position;
	float uv_x;
	glm::vec3 normal;
	float uv_y;
	glm::vec4 color;
};

// holds the resources needed for a mesh
struct GPUMeshBuffers
{

	AllocatedBuffer indexBuffer;
	AllocatedBuffer vertexBuffer;
	VkDeviceAddress vertexBufferAddress;
};

struct GPUDrawPushConstants
{
	glm::mat4 worldMatrix;
	VkDeviceAddress vertexBuffer;
};

struct DrawContext;

// base class for a renderable dynamic object
class IRenderable
{

	virtual void Draw(const glm::mat4& topMatrix, DrawContext& ctx) = 0;
};

// implementation of a drawable scene node.
// the scene node can hold children and will also keep a transform to propagate
// to them
struct Node : public IRenderable
{

	// parent pointer must be a weak pointer to avoid circular dependencies
	std::weak_ptr<Node> parent;
	std::vector<std::shared_ptr<Node>> children;

	glm::mat4 localTransform;
	glm::mat4 worldTransform;

	void refreshTransform(const glm::mat4& parentMatrix)
	{
		worldTransform = parentMatrix * localTransform;
		for (auto c : children)
		{
			c->refreshTransform(worldTransform);
		}
	}

	virtual void Draw(const glm::mat4& topMatrix, DrawContext& ctx)
	{
		// draw children
		for (auto& c : children)
		{
			c->Draw(topMatrix, ctx);
		}
	}
};

#define VK_CHECK(x)                                                                                                                                                           \
	do                                                                                                                                                                        \
	{                                                                                                                                                                         \
		VkResult err = x;                                                                                                                                                     \
		if (err != VK_SUCCESS)                                                                                                                                                \
		{                                                                                                                                                                     \
			SLIME_ERROR("Detected Vulkan error: %s", string_VkResult(err));                                                                                                   \
		}                                                                                                                                                                     \
	}                                                                                                                                                                         \
	while (0)