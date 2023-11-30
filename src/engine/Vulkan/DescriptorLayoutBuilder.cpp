#include "DescriptorLayoutBuilder.h"

#include "VulkanTypes.h"

void DescriptorLayoutBuilder::AddBinding(uint32_t binding, VkDescriptorType type, VkShaderStageFlags stageFlags)
{
	VkDescriptorSetLayoutBinding newbind{};
	newbind.binding         = binding;
	newbind.descriptorCount = 1;
	newbind.descriptorType  = type;
	newbind.stageFlags      = stageFlags;

	m_bindings.push_back(newbind);
}

void DescriptorLayoutBuilder::Clear()
{
	m_bindings.clear();
}

VkDescriptorSetLayout DescriptorLayoutBuilder::Build(VkDevice device)
{
	VkDescriptorSetLayoutCreateInfo info = {};
	info.sType                           = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	info.pNext                           = nullptr;

	info.pBindings    = m_bindings.data();
	info.bindingCount = (uint32_t)m_bindings.size();
	info.flags        = 0;

	VkDescriptorSetLayout set;
	VK_CHECK(vkCreateDescriptorSetLayout(device, &info, nullptr, &set));

	return set;
}

void DescriptorAllocator::InitPool(VkDevice device, uint32_t maxSets, std::span<PoolSizeRatio> poolRatios)
{
	std::vector<VkDescriptorPoolSize> poolSizes;
	for (const PoolSizeRatio& ratio : poolRatios)
	{
		poolSizes.push_back({ .type = ratio.type, .descriptorCount = static_cast<uint32_t>(ratio.ratio * maxSets) });
	}

	VkDescriptorPoolCreateInfo pool_info = {};
	pool_info.sType                      = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.maxSets                    = maxSets;
	pool_info.poolSizeCount              = static_cast<uint32_t>(poolSizes.size());
	pool_info.pPoolSizes                 = poolSizes.data();

	VK_CHECK(vkCreateDescriptorPool(device, &pool_info, nullptr, &m_pool));
}

void DescriptorAllocator::ClearDescriptors(VkDevice device)
{
	vkResetDescriptorPool(device, m_pool, 0);
}

void DescriptorAllocator::DestroyPool(VkDevice device)
{
	vkDestroyDescriptorPool(device, m_pool, nullptr);
}

VkDescriptorSet DescriptorAllocator::Allocate(VkDevice device, VkDescriptorSetLayout layout)
{
	VkDescriptorSetAllocateInfo alloc_info = {};
	alloc_info.sType                       = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	alloc_info.descriptorPool              = m_pool;
	alloc_info.descriptorSetCount          = 1;
	alloc_info.pSetLayouts                 = &layout;

	VkDescriptorSet set;
	VK_CHECK(vkAllocateDescriptorSets(device, &alloc_info, &set));

	return set;
}

bool DescriptorAllocator::UpdateDescriptors(VkDevice device, std::vector<VkWriteDescriptorSet> updates)
{
	for (const auto& update : updates)
	{
		if (update.descriptorCount == 0)
		{
			SLIME_WARN("Descriptor count is 0, skipping update");
			continue;
		}

		if (update.descriptorType == VK_DESCRIPTOR_TYPE_SAMPLER || update.descriptorType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER ||
		    update.descriptorType == VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE || update.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_IMAGE ||
		    update.descriptorType == VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT)
		{
			if (update.pImageInfo == nullptr)
			{
				SLIME_WARN("Image info is null, skipping update");
				continue;
			}
		}
		else if (update.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER || update.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER)
		{
			if (update.pTexelBufferView == nullptr)
			{
				SLIME_WARN("Texel buffer view is null, skipping update");
				continue;
			}
		}
		else
		{
			if (update.pBufferInfo == nullptr)
			{
				SLIME_WARN("Buffer info is null, skipping update");
				continue;
			}
		}
	}

	vkUpdateDescriptorSets(device, static_cast<uint32_t>(updates.size()), updates.data(), 0, nullptr);
	return true;
}

void DescriptorAllocator::UpdateImageDescriptor(VkDevice device, VkDescriptorSet set, VkImageView imageView, VkImageLayout imageLayout, uint32_t binding)
{
	VkDescriptorImageInfo imageInfo = {};
	imageInfo.imageView             = imageView;
	imageInfo.imageLayout           = imageLayout;

	VkWriteDescriptorSet writeDescriptorSet = {};
	writeDescriptorSet.sType                = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptorSet.dstSet               = set;
	writeDescriptorSet.dstBinding           = binding;
	writeDescriptorSet.dstArrayElement      = 0;
	writeDescriptorSet.descriptorType       = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	writeDescriptorSet.descriptorCount      = 1;
	writeDescriptorSet.pImageInfo           = &imageInfo;

	UpdateDescriptors(device, { writeDescriptorSet });
}