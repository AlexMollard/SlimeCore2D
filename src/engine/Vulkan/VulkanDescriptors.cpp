#include "VulkanDescriptors.h"

#include "VulkanEngine.h"
#include "VulkanInit.h"

using namespace vkutil;
void DescriptorLayoutBuilder::AddBinding(uint32_t binding, VkDescriptorType type)
{
	VkDescriptorSetLayoutBinding newbind{};
	newbind.binding         = binding;
	newbind.descriptorCount = 1;
	newbind.descriptorType  = type;

	bindings.push_back(newbind);
}

void DescriptorLayoutBuilder::Clear()
{
	bindings.clear();
}

VkDescriptorSetLayout DescriptorLayoutBuilder::Build(VkDevice device, VkShaderStageFlags shaderStages)
{
	for (auto& b : bindings)
	{
		b.stageFlags |= shaderStages;
	}

	VkDescriptorSetLayoutCreateInfo info = {};
	info.sType                           = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	info.pNext                           = nullptr;

	info.pBindings    = bindings.data();
	info.bindingCount = (uint32_t)bindings.size();
	info.flags        = 0;

	VkDescriptorSetLayout set;
	VK_CHECK(vkCreateDescriptorSetLayout(device, &info, nullptr, &set));

	return set;
}

void DescriptorAllocator::InitPool(VkDevice device, uint32_t maxSets, std::span<PoolSizeRatio> poolRatios)
{
	std::vector<VkDescriptorPoolSize> poolSizes;
	for (PoolSizeRatio ratio : poolRatios)
	{
		poolSizes.push_back(VkDescriptorPoolSize{ .type = ratio.type, .descriptorCount = uint32_t(ratio.ratio * maxSets) });
	}

	VkDescriptorPoolCreateInfo pool_info = {};
	pool_info.sType                      = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.flags                      = 0;
	pool_info.maxSets                    = maxSets;
	pool_info.poolSizeCount              = (uint32_t)poolSizes.size();
	pool_info.pPoolSizes                 = poolSizes.data();

	vkCreateDescriptorPool(device, &pool_info, nullptr, &pool);
}

void DescriptorAllocator::ClearDescriptors(VkDevice device)
{
	vkResetDescriptorPool(device, pool, 0);
}

void DescriptorAllocator::DestroyPool(VkDevice device)
{
	vkDestroyDescriptorPool(device, pool, nullptr);
}

VkDescriptorSet DescriptorAllocator::Allocate(VkDevice device, VkDescriptorSetLayout layout)
{
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.pNext                       = nullptr;
	allocInfo.sType                       = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool              = pool;
	allocInfo.descriptorSetCount          = 1;
	allocInfo.pSetLayouts                 = &layout;

	VkDescriptorSet ds;
	VK_CHECK(vkAllocateDescriptorSets(device, &allocInfo, &ds));

	return ds;
}

void DescriptorWriter::WriteImage(int binding, VkImageView image, VkSampler sampler, VkImageLayout layout, VkDescriptorType type)
{
	VkDescriptorImageInfo& info = imageInfos.emplace_back(VkDescriptorImageInfo{ .sampler = sampler, .imageView = image, .imageLayout = layout });

	writes.push_back(vkinit::WriteDescriptorImage(type, VK_NULL_HANDLE, &info, binding));
}

void DescriptorWriter::WriteBuffer(int binding, VkBuffer buffer, size_t size, size_t offset, VkDescriptorType type)
{
	VkDescriptorBufferInfo& info = bufferInfos.emplace_back(VkDescriptorBufferInfo{ .buffer = buffer, .offset = offset, .range = size });

	writes.push_back(vkinit::WriteDescriptorBuffer(type, VK_NULL_HANDLE, &info, binding));
}

void DescriptorWriter::Clear()
{
	imageInfos.clear();
	writes.clear();
	bufferInfos.clear();
}

void DescriptorWriter::Build(VkDevice device, VkDescriptorSet set)
{
	for (VkWriteDescriptorSet& write : writes)
	{
		write.dstSet = set;
	}

	vkUpdateDescriptorSets(device, (uint32_t)writes.size(), writes.data(), 0, nullptr);
}

void vkutil::InitDescriptors()
{
	VulkanEngine& engine = VulkanEngine::Get();

	// create a descriptor pool
	std::vector<DescriptorAllocator::PoolSizeRatio> sizes = {
		{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 3 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 3 },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 3 },
	};

	engine.globalDescriptorAllocator.InitPool(engine.m_device, 20, sizes);
	engine.AddToDeletionQueue([&]() { vkDestroyDescriptorPool(engine.m_device, engine.globalDescriptorAllocator.pool, nullptr); });

	{
		DescriptorLayoutBuilder builder;
		builder.AddBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
		engine.m_drawImageDescriptorLayout = builder.Build(engine.m_device, VK_SHADER_STAGE_COMPUTE_BIT);
	}
	{
		DescriptorLayoutBuilder builder;
		builder.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
		builder.AddBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
		engine.m_gpuSceneDataDescriptorLayout = builder.Build(engine.m_device, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
	}
	{
		DescriptorLayoutBuilder builder;
		builder.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
		builder.AddBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
		engine.m_gltfMatDescriptorLayout = builder.Build(engine.m_device, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	engine.AddToDeletionQueue(
	    [&]()
	    {
		    vkDestroyDescriptorSetLayout(engine.m_device, engine.m_drawImageDescriptorLayout, nullptr);
		    vkDestroyDescriptorSetLayout(engine.m_device, engine.m_gpuSceneDataDescriptorLayout, nullptr);
		    vkDestroyDescriptorSetLayout(engine.m_device, engine.m_gltfMatDescriptorLayout, nullptr);
	    });

	engine.m_drawImageDescriptors = engine.globalDescriptorAllocator.Allocate(engine.m_device, engine.m_drawImageDescriptorLayout);
	{
		DescriptorWriter writer;
		writer.WriteImage(0, engine.m_drawImage.imageView, VK_NULL_HANDLE, VK_IMAGE_LAYOUT_GENERAL, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
		writer.Build(engine.m_device, engine.m_drawImageDescriptors);
	}
	{
		// default white image descriptor
		uint32_t whitepixel = 0xFFFFFFFF;
		engine.m_whiteImage = engine.CreateImage((void*)&whitepixel, VkExtent3D{ 1, 1, 1 }, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT, "White Image");

		VkSamplerCreateInfo sampl = {};
		sampl.pNext               = nullptr;
		sampl.sType               = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;

		vkCreateSampler(engine.m_device, &sampl, nullptr, &engine.m_defaultSampler);

		engine.m_defaultGLTFdescriptor = engine.globalDescriptorAllocator.Allocate(engine.m_device, engine.m_gltfMatDescriptorLayout);

		engine.m_gltfDefaultOpaque.materialSet = engine.m_defaultGLTFdescriptor;

		// default material parameters
		engine.m_defaultGLTFMaterialData =
		    engine.CreateBuffer(sizeof(GPUGLTFMaterial), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU, "m_defaultGLTFMaterialData");

		DescriptorWriter writer;
		writer.WriteBuffer(0, engine.m_defaultGLTFMaterialData.buffer, sizeof(GPUGLTFMaterial), 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
		writer.WriteImage(1, engine.m_whiteImage.imageView, engine.m_defaultSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);

		writer.Build(engine.m_device, engine.m_defaultGLTFdescriptor);

		engine.AddToDeletionQueue(
		    [&]()
		    {
			    vkDestroySampler(engine.m_device, engine.m_defaultSampler, nullptr);
			    engine.DestroyBuffer(engine.m_defaultGLTFMaterialData);
			    engine.DestroyImage(engine.m_whiteImage);
		    });
	}
	for (int i = 0; i < FRAME_OVERLAP; i++)
	{
		// create a descriptor pool
		std::vector<DescriptorAllocator::PoolSizeRatio> frame_sizes = { { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 3 },
			                                                            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 3 },
			                                                            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3 } };

		engine.m_frames[i].m_frameDescriptors = DescriptorAllocator{};
		engine.m_frames[i].m_frameDescriptors.InitPool(engine.m_device, 1000, frame_sizes);

		engine.m_frames[i].cameraBuffer = engine.CreateBuffer(sizeof(GPUSceneData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

		engine.AddToDeletionQueue(
		    [&, i]()
		    {
			    engine.DestroyBuffer(engine.m_frames[i].cameraBuffer);
			    vkDestroyDescriptorPool(engine.m_device, engine.m_frames[i].m_frameDescriptors.pool, nullptr);
		    });
	}
}
