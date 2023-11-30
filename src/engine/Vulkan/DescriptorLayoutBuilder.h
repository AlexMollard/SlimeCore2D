#pragma once
#include <vector>
#include <vulkan/vulkan.h>
#include <span>

struct DescriptorLayoutBuilder
{
	void AddBinding(uint32_t binding, VkDescriptorType type, VkShaderStageFlags stageFlags);
	void Clear();
	VkDescriptorSetLayout Build(VkDevice device);

private:
	std::vector<VkDescriptorSetLayoutBinding> m_bindings;
};

struct DescriptorAllocator
{
	struct PoolSizeRatio
	{
		VkDescriptorType type;
		float ratio;
	};

    void InitPool(VkDevice device, uint32_t maxSets, std::span<PoolSizeRatio> poolRatios);
	void ClearDescriptors(VkDevice device);
	void DestroyPool(VkDevice device);
	VkDescriptorSet Allocate(VkDevice device, VkDescriptorSetLayout layout);
	bool UpdateDescriptors(VkDevice device, std::vector<VkWriteDescriptorSet> updates);
	void UpdateImageDescriptor(VkDevice device, VkDescriptorSet set, VkImageView imageView, VkImageLayout imageLayout, uint32_t binding);

private:
	VkDescriptorPool m_pool;
};
