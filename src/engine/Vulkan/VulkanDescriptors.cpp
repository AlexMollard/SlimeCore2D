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

uint32_t vkutil::GetPadSizeToMinAlignment(uint32_t originalSize)
{
	VulkanEngine& engine = VulkanEngine::Get();

	VkPhysicalDeviceProperties properties;
	vkGetPhysicalDeviceProperties(engine.m_chosenGPU, &properties);

	auto minUniformBufferOffsetAlignment = static_cast<uint32_t>(properties.limits.minUniformBufferOffsetAlignment);

	if (minUniformBufferOffsetAlignment > 0)
	{
		return (originalSize + minUniformBufferOffsetAlignment - 1) & ~(minUniformBufferOffsetAlignment - 1);
	}
	else
	{
		return originalSize;
	}
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

	// Setup the BindlessDescriptorWriter
	engine.m_bindlessWriter.Build();
	engine.AddToDeletionQueue([&]() { engine.m_bindlessWriter.Clear(); });

	struct Camera
	{
		glm::mat4 viewProjection;
		glm::mat4 view;
		glm::mat4 projection;
	};



	BindlessParams params{ 256 };
	params.addRange(Camera({}));
	params.build(engine.m_device, engine.m_allocator, engine.m_bindlessWriter.bindlessPool);
}

void BindlessDescriptorWriter::Build()
{
	VulkanEngine& engine = VulkanEngine::Get();

	std::array<VkDescriptorSetLayoutBinding, 3> bindings{};
	std::array<VkDescriptorBindingFlags, 3> flags{};
	std::array<VkDescriptorType, 3> types{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER };

	for (uint32_t i = 0; i < 3; ++i)
	{
		bindings.at(i).binding        = i;
		bindings.at(i).descriptorType = types.at(i);
		// Due to partially bound bit, this value
		// is used as an upper bound, which we have set to
		// 1000 to keep it simple for the sake of this post
		bindings.at(i).descriptorCount = 1000;
		bindings.at(i).stageFlags      = VK_SHADER_STAGE_ALL;
		flags.at(i)                    = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;
	}

	VkDescriptorSetLayoutBindingFlagsCreateInfo bindingFlags{};
	bindingFlags.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
	bindingFlags.pNext         = nullptr;
	bindingFlags.pBindingFlags = flags.data();
	bindingFlags.bindingCount  = 3;

	VkDescriptorSetLayoutCreateInfo createInfo{};
	createInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	createInfo.bindingCount = 3;
	createInfo.pBindings    = bindings.data();
	createInfo.flags        = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;
	createInfo.pNext        = &bindingFlags;

	// Create layout
	vkCreateDescriptorSetLayout(engine.m_device, &createInfo, nullptr, &bindlessLayout);

	// Create pool
	VkDescriptorPoolSize poolSize{};
	poolSize.type            = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize.descriptorCount = 1000;

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.maxSets       = 1000;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes    = &poolSize;
	poolInfo.flags         = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;

	vkCreateDescriptorPool(engine.m_device, &poolInfo, nullptr, &bindlessPool);

	// Allocate descriptor set
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool     = bindlessPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts        = &bindlessLayout;

	vkAllocateDescriptorSets(engine.m_device, &allocInfo, &bindlessDescriptor);
}

vkutil::TextureHandle BindlessDescriptorWriter::StoreTexture(VkImageView imageView, VkSampler sampler)
{
	size_t newHandle = textures.size();
	textures.push_back(imageView);

	VkDescriptorImageInfo imageInfo{};
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfo.imageView   = imageView;
	imageInfo.sampler     = sampler;

	VkWriteDescriptorSet write{};
	write.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	write.dstBinding      = TextureBinding;
	write.dstSet          = bindlessDescriptor;
	write.descriptorCount = 1;
	write.dstArrayElement = newHandle;
	write.pImageInfo      = &imageInfo;

	vkUpdateDescriptorSets(VulkanEngine::Get().m_device, 1, &write, 0, nullptr);
	return static_cast<TextureHandle>(newHandle);
}

vkutil::BufferHandle BindlessDescriptorWriter::StoreBuffer(VkBuffer buffer, VkBufferUsageFlagBits usage)
{
	size_t newHandle = buffers.size();
	buffers.push_back(buffer);

	std::array<VkWriteDescriptorSet, 2> writes{};
	for (auto& write : writes)
	{
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = buffer;
		bufferInfo.offset = 0;
		bufferInfo.range  = VK_WHOLE_SIZE;

		write.dstSet = bindlessDescriptor;
		// Write one buffer that is being added
		write.descriptorCount = 1;
		// The array element that we are going to write to
		// is the index, which we refer to as our handles
		write.dstArrayElement = newHandle;
		write.pBufferInfo     = &bufferInfo;
	}

	size_t index = 0;
	if ((usage & VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT) == VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)
	{
		writes.at(index).dstBinding     = UniformBinding;
		writes.at(index).descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		index++;
	}

	if ((usage & VK_BUFFER_USAGE_STORAGE_BUFFER_BIT) == VK_BUFFER_USAGE_STORAGE_BUFFER_BIT)
	{
		writes.at(index).dstBinding     = StorageBinding;
		writes.at(index).descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	}

	vkUpdateDescriptorSets(VulkanEngine::Get().m_device, index, writes.data(), 0, nullptr);

	return static_cast<BufferHandle>(newHandle);
}

void BindlessDescriptorWriter::Clear()
{
	textures.clear();
	buffers.clear();

	VulkanEngine& engine = VulkanEngine::Get();
	vkDestroyDescriptorSetLayout(engine.m_device, bindlessLayout, nullptr);
	vkDestroyDescriptorPool(engine.m_device, bindlessPool, nullptr);
}

VkDescriptorSetLayout BindlessParams::getDescriptorSetLayout()
{
	return m_layout;
}

VkDescriptorSet BindlessParams::getDescriptorSet()
{
	return m_descriptorSet;
}

void BindlessParams::build(VkDevice device, VmaAllocator allocator, VkDescriptorPool descriptorPool)
{
	VulkanEngine& engine = VulkanEngine::Get();

	size_t allocSize = 0;
	for (auto& range : m_ranges)
	{
		allocSize += range.size;
	}
	VkBufferUsageFlags usage   = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	VmaMemoryUsage memoryUsage = VMA_MEMORY_USAGE_CPU_TO_GPU;
	const char* name           = "BindlessParams";
	AllocatedBuffer buffer     = engine.CreateBuffer(allocSize, usage, memoryUsage, name);
	m_buffer 				 = buffer.buffer;

	// Copy ranges to buffer
	uint8_t* data = nullptr;
	vmaMapMemory(allocator, buffer.allocation, (void**)&data);
	for (const auto& range : m_ranges)
	{
		memcpy(data + range.offset, range.data, range.size);
	}
	vmaUnmapMemory(allocator, buffer.allocation);

	// Create layout for descriptor set
	VkDescriptorSetLayoutBinding binding{};
	binding.binding         = 0;
	binding.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	binding.descriptorCount = 1;
	binding.stageFlags      = VK_SHADER_STAGE_ALL;

	VkDescriptorSetLayoutCreateInfo createInfo{};
	createInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	createInfo.bindingCount = 1;
	createInfo.pBindings    = &binding;
	vkCreateDescriptorSetLayout(device, &createInfo, nullptr, &m_layout);

	// Get maximum size of a single range
	uint32_t maxRangeSize = 0;
	for (auto& range : m_ranges)
	{
		maxRangeSize = std::max(range.size, maxRangeSize);
	}

	// Create descriptor
	VkDescriptorSetAllocateInfo allocateInfo{};
	allocateInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocateInfo.pNext              = nullptr;
	allocateInfo.descriptorPool     = descriptorPool;
	allocateInfo.pSetLayouts        = &m_layout;
	allocateInfo.descriptorSetCount = 1;
	vkAllocateDescriptorSets(device, &allocateInfo, &m_descriptorSet);

	VkDescriptorBufferInfo bufferInfo{};
	bufferInfo.buffer = m_buffer;
	bufferInfo.offset = 0;
	bufferInfo.range  = maxRangeSize;

	VkWriteDescriptorSet write{};
	write.sType		   = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	write.descriptorType  = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	write.dstBinding      = 0;
	write.dstSet          = m_descriptorSet;
	write.descriptorCount = 1;
	write.dstArrayElement = 0;
	write.pBufferInfo     = &bufferInfo;
	vkUpdateDescriptorSets(device, 1, &write, 0, nullptr);
}

BindlessParams::BindlessParams(uint32_t minAlignment) : m_minAlignment(minAlignment) {}
