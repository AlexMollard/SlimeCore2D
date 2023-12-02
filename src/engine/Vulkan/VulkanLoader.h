#pragma once

#include "VulkanDescriptors.h"
#include <unordered_map>
#include "VulkanTypes.h"

class VulkanEngine;

struct GLTFMaterial
{
	MaterialData data;
};

struct GeoSurface
{
	uint32_t startIndex;
	uint32_t vertexOffset;
	uint32_t count;
	std::shared_ptr<GLTFMaterial> material;
};

struct MeshAsset
{
	std::string name;

	std::vector<GeoSurface> surfaces;
	GPUMeshBuffers meshBuffers;
};

struct MeshNode : public Node
{

	std::shared_ptr<MeshAsset> mesh;

	virtual void Draw(const glm::mat4& topMatrix, DrawContext& ctx);
};

struct LoadedGLTF : public IRenderable
{

	// storage for all the data on a given gltf file
	std::unordered_map<std::string, std::shared_ptr<MeshAsset>> meshes;
	std::unordered_map<std::string, std::shared_ptr<Node>> nodes;
	std::unordered_map<std::string, AllocatedImage> images;
	std::unordered_map<std::string, std::shared_ptr<GLTFMaterial>> materials;

	// nodes that dont have a parent, for iterating through the file in tree order
	std::vector<std::shared_ptr<Node>> topNodes;

	std::vector<VkSampler> samplers;

	DescriptorAllocator descriptorPool;

	AllocatedBuffer materialDataBuffer;

	// gltf defaults to a default white image for things not found
	AllocatedImage defaultImage;

	~LoadedGLTF()
	{
		ClearAll();
	};

	virtual void Draw(const glm::mat4& topMatrix, DrawContext& ctx);

private:
	void ClearAll();
};

std::optional<std::shared_ptr<LoadedGLTF>> LoadGltf(std::string_view filePath);