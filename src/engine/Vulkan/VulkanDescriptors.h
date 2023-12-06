#pragma once

#include "VulkanTypes.h"
#include <deque>
#include <span>
#include <vector>

namespace vkutil
{
uint32_t GetPadSizeToMinAlignment(uint32_t originalSize);

struct DescriptorLayoutBuilder
{
	std::vector<VkDescriptorSetLayoutBinding> bindings;

	void AddBinding(uint32_t binding, VkDescriptorType type);
	void Clear();
	VkDescriptorSetLayout Build(VkDevice device, VkShaderStageFlags shaderStages);
};

struct DescriptorWriter
{
	std::deque<VkDescriptorImageInfo> imageInfos;
	std::deque<VkDescriptorBufferInfo> bufferInfos;
	std::vector<VkWriteDescriptorSet> writes;

	void WriteImage(int binding, VkImageView image, VkSampler sampler, VkImageLayout layout, VkDescriptorType type);
	void WriteBuffer(int binding, VkBuffer buffer, size_t size, size_t offset, VkDescriptorType type);

	void Clear();
	void Build(VkDevice device, VkDescriptorSet set);
};

struct DescriptorAllocator
{
	struct PoolSizeRatio
	{
		VkDescriptorType type;
		float ratio;
	};

	VkDescriptorPool pool;

	void InitPool(VkDevice device, uint32_t maxSets, std::span<PoolSizeRatio> poolRatios);
	void ClearDescriptors(VkDevice device);
	void DestroyPool(VkDevice device);

	VkDescriptorSet Allocate(VkDevice device, VkDescriptorSetLayout layout);
};

void InitDescriptors();

// ========================================================================================================
// BINDLESS DESCRIPTORS
// ========================================================================================================
enum class TextureHandle : uint32_t
{
	Invalid = 0
};

enum class BufferHandle : uint32_t
{
	Invalid = 0
};

struct BindlessDescriptorWriter
{
	static constexpr uint32_t UniformBinding = 0;
	static constexpr uint32_t StorageBinding = 1;
	static constexpr uint32_t TextureBinding = 2;

	VkDescriptorSetLayout bindlessLayout = VK_NULL_HANDLE;
	VkDescriptorPool bindlessPool        = VK_NULL_HANDLE;
	VkDescriptorSet bindlessDescriptor   = VK_NULL_HANDLE;

	std::vector<VkImageView> textures;
	std::vector<VkBuffer> buffers;

	TextureHandle StoreTexture(VkImageView imageView, VkSampler sampler);
	BufferHandle StoreBuffer(VkBuffer buffer, VkBufferUsageFlagBits usage);

	void Build();
	void Clear();
};

class BindlessParams
{
	struct Range
	{
		uint32_t offset;
		uint32_t size;
		void *data;
	};

public:
	BindlessParams(uint32_t minAlignment);

	template<class TData>
	uint32_t addRange(TData &&data);
	void build(VkDevice device, VmaAllocator allocator, VkDescriptorPool descriptorPool);

	VkDescriptorSet getDescriptorSet();
	VkDescriptorSetLayout getDescriptorSetLayout();

private:
	uint32_t m_minAlignment;
	uint32_t m_lastOffset = 0;
	std::vector<Range> m_ranges;

	VkDescriptorSetLayout m_layout;
	VkDescriptorSet m_descriptorSet;

	VmaAllocation m_allocation;
	VkBuffer m_buffer;
};

template<class TData>
uint32_t vkutil::BindlessParams::addRange(TData &&data)
{
	// Copy data to heap and store void pointer
	// since we do not care about the type at
	// point
	uint32_t dataSize = sizeof(TData);
	auto *bytes     = new TData;
	*bytes          = data;

	// Add range
	uint32_t currentOffset = m_lastOffset;
	m_ranges.push_back({ currentOffset, dataSize, bytes });

	// Pad the data size to minimum alignment
	// and move the offset
	m_lastOffset += GetPadSizeToMinAlignment(dataSize);
	return currentOffset;
}

} // namespace vkutil