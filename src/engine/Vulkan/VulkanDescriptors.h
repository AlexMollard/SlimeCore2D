#pragma once

#include "VulkanTypes.h"
#include <deque>
#include <span>
#include <vector>

namespace vkutil
{
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
} // namespace vkutil