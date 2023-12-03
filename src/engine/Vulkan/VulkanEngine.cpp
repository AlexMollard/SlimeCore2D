#include "VulkanEngine.h"

#define VMA_IMPLEMENTATION
#define VMA_DEBUG_INITIALIZE_ALLOCATIONS 1
#define VMA_DEBUG_ALWAYS_DEDICATED_MEMORY 1
#define VMA_DEBUG_LOG(format, ...) printf(format, __VA_ARGS__); printf("\n")
#include "vulkan/vk_mem_alloc.h"

#include "Pipeline.h"
#include "VkBootstrap.h"
#include "VulkanImages.h"
#include "VulkanInit.h"
#include "VulkanLoader.h"
#include "VulkanTypes.h"
#include "gtx/transform.hpp"
#include "imgui/backends/imgui_impl_sdl3.h"
#include "imgui/backends/imgui_impl_vulkan.h"
#include "imgui/imgui.h"
#include <SDL3/SDL_vulkan.h>

#include "engine/ConsoleLog.h"
#include "engine/MemoryDebugging.h"
#include "engine/ResourceManager.h"

constexpr uint64_t ONE_SECOND = 1000000000;

void VulkanEngine::Init()
{
	// We initialize SDL and create a window with it.
	SDL_Init(SDL_INIT_VIDEO);

	SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);

	// create blank SDL window for our application
	m_window = SDL_CreateWindow("Vulkan Engine",       // window title
	                            m_windowExtent.width,  // window width in pixels
	                            m_windowExtent.height, // window height in pixels
	                            window_flags);

	// Initialize Vulkan
	InitVulkan();

	// Initialize Swapchain
	InitSwapchain();

	// Initialize Commands
	InitCommands();

	// Initialize Sync Structures
	InitSyncStructures();

	// Initialize Descriptors
	InitDescriptors();

	// Initialize Pipelines
	InitPipelines();

	// Initialize Imgui
	InitImgui();

	m_isInitialized = true;

	mainCamera.velocity = glm::vec3(0.0f);
	mainCamera.position = glm::vec3(0.0f, 0.0f, 20.0f);

	mainCamera.pitch = 0;
	mainCamera.yaw   = 0;
}

void VulkanEngine::InitSwapchain()
{
	vkb::SwapchainBuilder swapchainBuilder{ m_chosenGPU, m_device, m_surface };

	vkb::Swapchain vkbSwapchain = swapchainBuilder
	                                  //.use_default_format_selection()
	                                  .set_desired_format(VkSurfaceFormatKHR{ .format = VK_FORMAT_B8G8R8A8_UNORM, .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR })
	                                  // use vsync present mode
	                                  .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
	                                  .set_desired_extent(m_windowExtent.width, m_windowExtent.height)
	                                  .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
	                                  .build()
	                                  .value();

	// store swapchain and its related images
	m_swapchain           = vkbSwapchain.swapchain;
	m_swapchainImages     = vkbSwapchain.get_images().value();
	m_swapchainImageViews = vkbSwapchain.get_image_views().value();

	m_swapchainImageFormat = vkbSwapchain.image_format;

	// depth image size will match the window
	VkExtent3D drawImageExtent = { m_windowExtent.width, m_windowExtent.height, 1 };

	// hardcoding the draw format to 32 bit float
	m_drawImage.imageFormat = VK_FORMAT_R16G16B16A16_SFLOAT;

	VkImageUsageFlags drawImageUsages{};
	drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	drawImageUsages |= VK_IMAGE_USAGE_STORAGE_BIT;
	drawImageUsages |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	VkImageCreateInfo rimg_info = vkinit::ImageCreateInfo(m_drawImage.imageFormat, drawImageUsages, drawImageExtent);

	// for the draw image, we want to allocate it from gpu local memory
	VmaAllocationCreateInfo rimg_allocinfo = {};
	rimg_allocinfo.usage                   = VMA_MEMORY_USAGE_GPU_ONLY;
	rimg_allocinfo.requiredFlags           = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	// allocate and create the image
	vmaCreateImage(m_allocator, &rimg_info, &rimg_allocinfo, &m_drawImage.image, &m_drawImage.allocation, nullptr);

	// build a image-view for the draw image to use for rendering
	VkImageViewCreateInfo rview_info = vkinit::ImageviewCreateInfo(m_drawImage.imageFormat, m_drawImage.image, VK_IMAGE_ASPECT_COLOR_BIT);

	VK_CHECK(vkCreateImageView(m_device, &rview_info, nullptr, &m_drawImage.imageView));

	// create a depth image too
	m_depthImage.imageFormat = VK_FORMAT_D32_SFLOAT;

	VkImageUsageFlags depthImageUsages{};
	depthImageUsages |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

	VkImageCreateInfo dimg_info = vkinit::ImageCreateInfo(m_depthImage.imageFormat, depthImageUsages, drawImageExtent);

	// allocate and create the image
	vmaCreateImage(m_allocator, &dimg_info, &rimg_allocinfo, &m_depthImage.image, &m_depthImage.allocation, nullptr);

	// build a image-view for the draw image to use for rendering
	VkImageViewCreateInfo dview_info = vkinit::ImageviewCreateInfo(m_depthImage.imageFormat, m_depthImage.image, VK_IMAGE_ASPECT_DEPTH_BIT);

	VK_CHECK(vkCreateImageView(m_device, &dview_info, nullptr, &m_depthImage.imageView));

	// add to deletion queues
	m_mainDeletionQueue.push_function(
	    [=]()
	    {
		    vkDestroyImageView(m_device, m_drawImage.imageView, nullptr);
		    vmaDestroyImage(m_allocator, m_drawImage.image, m_drawImage.allocation);

		    vkDestroyImageView(m_device, m_depthImage.imageView, nullptr);
		    vmaDestroyImage(m_allocator, m_depthImage.image, m_depthImage.allocation);
	    });
}

void VulkanEngine::InitVulkan()
{
	vkb::InstanceBuilder builder;

	// If we are in debug enable validation layers
#ifdef _DEBUG
	bool useValidationLayers = true;
#else
	bool useValidationLayers = false;
#endif // DEBUG

	auto debugCallBackOutPut = [](VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	                              VkDebugUtilsMessageTypeFlagsEXT messageType,
	                              const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	                              void* pUserData) -> VkBool32
	{
		auto severity = vkb::to_string_message_severity(messageSeverity);
		auto type     = vkb::to_string_message_type(messageType);

		ConsoleColor color = ConsoleColor::White;

		// Set the color depending on the warning
		if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
			color = ConsoleColor::White;
		else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
			color = ConsoleColor::Blue;
		else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
			color = ConsoleColor::Yellow;
		else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
			color = ConsoleColor::Red;

		std::stringstream ss;
		ss << "[" << severity << ": " << type << "]";

		ConsoleLog::log(ss.str(), color);
		ConsoleLog::log(pCallbackData->pMessage, ConsoleColor::White);
		std::cout << std::endl;

		if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
			DebugBreak();

		return VK_FALSE;
	};

	// make the vulkan instance, with basic debug features
	auto instRet = builder
	                   .set_app_name("Example Vulkan Application")     // name of the application
	                   .request_validation_layers(useValidationLayers) // enable validation layers
	                   .set_debug_callback(debugCallBackOutPut)        // enable default debug messenger
	                   .require_api_version(1, 3, 0)                   // require Vulkan 1.3.0 or higher
	                   .build();                                       // build the instance

	vkb::Instance vkbInst = instRet.value();

	// grab the instance
	m_instance        = vkbInst.instance;
	m_debug_messenger = vkbInst.debug_messenger;

	SDL_Vulkan_CreateSurface(m_window, m_instance, nullptr, &m_surface);

	// VkPhysicalDeviceVulkan11Features features11{};

	VkPhysicalDeviceVulkan12Features features12{};
	features12.bufferDeviceAddress = true;
	features12.descriptorIndexing  = true;

	VkPhysicalDeviceVulkan13Features features13{};
	features13.dynamicRendering = true;
	features13.synchronization2 = true;

	// use vkbootstrap to select a gpu.
	// We want a gpu that can write to the SDL surface and supports vulkan 1.3
	vkb::PhysicalDeviceSelector selector{ vkbInst };

	vkb::PhysicalDevice physicalDevice = selector
	                                         .set_minimum_version(1, 3)            // require Vulkan 1.3.0 or higher
	                                         .set_required_features_12(features12) // require Vulkan 1.3.0 or higher
	                                         .set_required_features_13(features13) // require Vulkan 1.3.0 or higher
	                                         .set_surface(m_surface)               // Set the current surface as this is needed to determine what the gpu requires
	                                         .select()                             // Select the best gpu
	                                         .value();                             // Get the selected gpu

	// create the final vulkan device
	vkb::DeviceBuilder deviceBuilder{ physicalDevice };
	vkb::Device vkbDevice = deviceBuilder.build().value();

	// Get the VkDevice handle used in the rest of a vulkan application
	m_device    = vkbDevice.device;
	m_chosenGPU = physicalDevice.physical_device;

	// use vkbootstrap to get a Graphics queue
	m_graphicsQueue       = vkbDevice.get_queue(vkb::QueueType::graphics).value();
	m_graphicsQueueFamily = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();

	// initialize the memory allocator
	VmaAllocatorCreateInfo allocatorInfo = {};
	allocatorInfo.physicalDevice         = m_chosenGPU;
	allocatorInfo.device                 = m_device;
	allocatorInfo.instance               = m_instance;
	allocatorInfo.flags                  = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
	vmaCreateAllocator(&allocatorInfo, &m_allocator);

	// m_mainDeletionQueue.push_function([=]() { vmaDestroyAllocator(m_allocator); });
}

void VulkanEngine::InitCommands()
{
	// create a command pool for commands submitted to the graphics queue.
	// we also want the pool to allow for resetting of individual command buffers
	VkCommandPoolCreateInfo commandPoolInfo = vkinit::CommandPoolCreateInfo(m_graphicsQueueFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

	for (int i = 0; i < FRAME_OVERLAP; i++)
	{

		VK_CHECK(vkCreateCommandPool(m_device, &commandPoolInfo, nullptr, &m_frames[i].m_commandPool));

		// allocate the default command buffer that we will use for rendering
		VkCommandBufferAllocateInfo cmdAllocInfo = vkinit::CommandBufferAllocateInfo(m_frames[i].m_commandPool, 1);

		VK_CHECK(vkAllocateCommandBuffers(m_device, &cmdAllocInfo, &m_frames[i].m_mainCommandBuffer));
	}

	VK_CHECK(vkCreateCommandPool(m_device, &commandPoolInfo, nullptr, &m_immCommandPool));

	// allocate the default command buffer that we will use for rendering
	VkCommandBufferAllocateInfo cmdAllocInfo = vkinit::CommandBufferAllocateInfo(m_immCommandPool, 1);

	VK_CHECK(vkAllocateCommandBuffers(m_device, &cmdAllocInfo, &m_immCommandBuffer));

	m_mainDeletionQueue.push_function([=]() { vkDestroyCommandPool(m_device, m_immCommandPool, nullptr); });
}

void VulkanEngine::InitSyncStructures()
{
	// create syncronization structures
	// one fence to control when the gpu has finished rendering the frame,
	// and 2 semaphores to syncronize rendering with swapchain
	// we want the fence to start signalled so we can wait on it on the first frame
	VkFenceCreateInfo fenceCreateInfo = vkinit::FenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);
	VK_CHECK(vkCreateFence(m_device, &fenceCreateInfo, nullptr, &m_immFence));

	m_mainDeletionQueue.push_function([=]() { vkDestroyFence(m_device, m_immFence, nullptr); });

	for (int i = 0; i < FRAME_OVERLAP; i++)
	{

		VK_CHECK(vkCreateFence(m_device, &fenceCreateInfo, nullptr, &m_frames[i].m_renderFence));

		VkSemaphoreCreateInfo semaphoreCreateInfo = vkinit::SemaphoreCreateInfo();

		VK_CHECK(vkCreateSemaphore(m_device, &semaphoreCreateInfo, nullptr, &m_frames[i].m_swapchainSemaphore));
		VK_CHECK(vkCreateSemaphore(m_device, &semaphoreCreateInfo, nullptr, &m_frames[i].m_renderSemaphore));

		m_mainDeletionQueue.push_function(
		    [=]()
		    {
			    vkDestroyFence(m_device, m_frames[i].m_renderFence, nullptr);
			    vkDestroySemaphore(m_device, m_frames[i].m_swapchainSemaphore, nullptr);
			    vkDestroySemaphore(m_device, m_frames[i].m_renderSemaphore, nullptr);
		    });
	}
}

void VulkanEngine::InitDescriptors()
{
	// create a descriptor pool
	std::vector<DescriptorAllocator::PoolSizeRatio> sizes = {
		{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 3 },
		{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 3 },
		{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 3 },
	};

	globalDescriptorAllocator.InitPool(m_device, 20, sizes);
	m_mainDeletionQueue.push_function([&]() { vkDestroyDescriptorPool(m_device, globalDescriptorAllocator.pool, nullptr); });

	{
		DescriptorLayoutBuilder builder;
		builder.AddBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
		m_drawImageDescriptorLayout = builder.Build(m_device, VK_SHADER_STAGE_COMPUTE_BIT);
	}
	{
		DescriptorLayoutBuilder builder;
		builder.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
		m_gpuSceneDataDescriptorLayout = builder.Build(m_device, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
	}
	{
		DescriptorLayoutBuilder builder;
		builder.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
		builder.AddBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
		m_gltfMatDescriptorLayout = builder.Build(m_device, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
	}

	m_mainDeletionQueue.push_function(
	    [&]()
	    {
		    vkDestroyDescriptorSetLayout(m_device, m_drawImageDescriptorLayout, nullptr);
		    vkDestroyDescriptorSetLayout(m_device, m_gpuSceneDataDescriptorLayout, nullptr);
		    vkDestroyDescriptorSetLayout(m_device, m_gltfMatDescriptorLayout, nullptr);
	    });

	m_drawImageDescriptors = globalDescriptorAllocator.Allocate(m_device, m_drawImageDescriptorLayout);
	{
		DescriptorWriter writer;
		writer.WriteImage(0, m_drawImage.imageView, VK_NULL_HANDLE, VK_IMAGE_LAYOUT_GENERAL, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
		writer.Build(m_device, m_drawImageDescriptors);
	}
	{
		// default white image descriptor
		uint32_t whitepixel = 0xFFFFFFFF;
		m_whiteImage        = CreateImage((void*)&whitepixel, VkExtent3D{ 1, 1, 1 }, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT);

		VkSamplerCreateInfo sampl = {};
		sampl.pNext               = nullptr;
		sampl.sType               = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;

		vkCreateSampler(m_device, &sampl, nullptr, &m_defaultSampler);

		m_defaultGLTFdescriptor = globalDescriptorAllocator.Allocate(m_device, m_gltfMatDescriptorLayout);

		m_gltfDefaultOpaque.materialSet = m_defaultGLTFdescriptor;

		// default material parameters
		m_defaultGLTFMaterialData = CreateBuffer(sizeof(GPUGLTFMaterial), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

		DescriptorWriter writer;
		writer.WriteBuffer(0, m_defaultGLTFMaterialData.buffer, sizeof(GPUGLTFMaterial), 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
		writer.WriteImage(1, m_whiteImage.imageView, m_defaultSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);

		writer.Build(m_device, m_defaultGLTFdescriptor);

		m_mainDeletionQueue.push_function(
		    [&]()
		    {
			    vkDestroySampler(m_device, m_defaultSampler, nullptr);
			    DestroyBuffer(m_defaultGLTFMaterialData);
			    DestroyImage(m_whiteImage);
		    });
	}
	for (int i = 0; i < FRAME_OVERLAP; i++)
	{
		// create a descriptor pool
		std::vector<DescriptorAllocator::PoolSizeRatio> frame_sizes = { { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 3 },
			                                                            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 3 },
			                                                            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3 } };

		m_frames[i].m_frameDescriptors = DescriptorAllocator{};
		m_frames[i].m_frameDescriptors.InitPool(m_device, 1000, frame_sizes);

		m_frames[i].cameraBuffer = CreateBuffer(sizeof(GPUSceneData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

		m_mainDeletionQueue.push_function(
		    [&, i]()
		    {
			    DestroyBuffer(m_frames[i].cameraBuffer);
			    vkDestroyDescriptorPool(m_device, m_frames[i].m_frameDescriptors.pool, nullptr);
		    });
	}
}

ComputeEffect* VulkanEngine::CreateComputeEffect(const char* name, VkShaderModule shader, VkPipelineLayout layout, VkComputePipelineCreateInfo computePipelineCreateInfo)
{
	ComputeEffect* effect = new ComputeEffect();
	effect->layout        = layout;
	effect->name          = name;
	effect->data          = {};

	computePipelineCreateInfo.stage.module = shader;
	VK_CHECK(vkCreateComputePipelines(m_device, VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr, &effect->pipeline));

	backgroundEffects.push_back(effect);
	vkDestroyShaderModule(m_device, shader, nullptr);

	return effect;
}

AllocatedImage VulkanEngine::CreateImage(VkExtent3D size, VkFormat format, VkImageUsageFlags usage)
{
	AllocatedImage newImage;
	newImage.imageFormat = format;
	newImage.imageExtent = size;

	VkImageCreateInfo img_info = vkinit::ImageCreateInfo(format, usage, size);

	// always allocate images on dedicated GPU memory
	VmaAllocationCreateInfo allocinfo = {};
	allocinfo.usage                   = VMA_MEMORY_USAGE_GPU_ONLY;
	allocinfo.requiredFlags           = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	// allocate and create the image
	VK_CHECK(vmaCreateImage(m_allocator, &img_info, &allocinfo, &newImage.image, &newImage.allocation, nullptr));

	// if the format is a depth format, we will need to have it use the correct
	// aspect flag
	VkImageAspectFlags aspectFlag = VK_IMAGE_ASPECT_COLOR_BIT;
	if (format == VK_FORMAT_D32_SFLOAT)
	{
		aspectFlag = VK_IMAGE_ASPECT_DEPTH_BIT;
	}

	// build a image-view for the image
	VkImageViewCreateInfo view_info = vkinit::ImageviewCreateInfo(format, newImage.image, aspectFlag);

	VK_CHECK(vkCreateImageView(m_device, &view_info, nullptr, &newImage.imageView));

	return newImage;
}

AllocatedImage VulkanEngine::CreateImage(void* data, VkExtent3D size, VkFormat format, VkImageUsageFlags usage)
{
	size_t data_size             = size.depth * size.width * size.height * 4;
	AllocatedBuffer uploadbuffer = CreateBuffer(data_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

	memcpy(uploadbuffer.info.pMappedData, data, data_size);

	AllocatedImage new_image = CreateImage(size, format, usage | VK_IMAGE_USAGE_TRANSFER_DST_BIT);

	ImmediateSubmit(
	    [&](VkCommandBuffer cmd)
	    {
		    vkutil::TransitionImage(cmd, new_image.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

		    VkBufferImageCopy copyRegion = {};
		    copyRegion.bufferOffset      = 0;
		    copyRegion.bufferRowLength   = 0;
		    copyRegion.bufferImageHeight = 0;

		    copyRegion.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
		    copyRegion.imageSubresource.mipLevel       = 0;
		    copyRegion.imageSubresource.baseArrayLayer = 0;
		    copyRegion.imageSubresource.layerCount     = 1;
		    copyRegion.imageExtent                     = size;

		    // copy the buffer into the image
		    vkCmdCopyBufferToImage(cmd, uploadbuffer.buffer, new_image.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

		    vkutil::TransitionImage(cmd, new_image.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	    });

	DestroyBuffer(uploadbuffer);

	return new_image;
}

void VulkanEngine::DestroyImage(AllocatedImage image)
{
	vkDestroyImageView(m_device, image.imageView, nullptr);
	vmaDestroyImage(m_allocator, image.image, image.allocation);

	image.image      = VK_NULL_HANDLE;
	image.imageView  = VK_NULL_HANDLE;
	image.allocation = VK_NULL_HANDLE;
}

void VulkanEngine::InitPipelines()
{
	// Compute pipelines
	InitBackgroundPipelines();

	// Graphics pipelines
	InitMeshPipeline();

	InitDefaultData();
}

void VulkanEngine::InitBackgroundPipelines()
{
	VkPipelineLayoutCreateInfo computeLayout{};
	computeLayout.sType          = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	computeLayout.pNext          = nullptr;
	computeLayout.pSetLayouts    = &m_drawImageDescriptorLayout;
	computeLayout.setLayoutCount = 1;

	VkPushConstantRange pushConstant{};
	pushConstant.offset     = 0;
	pushConstant.size       = sizeof(ComputePushConstants);
	pushConstant.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

	computeLayout.pPushConstantRanges    = &pushConstant;
	computeLayout.pushConstantRangeCount = 1;

	VK_CHECK(vkCreatePipelineLayout(m_device, &computeLayout, nullptr, &m_gradientPipelineLayout));

	VkShaderModule gradientShader   = vkutil::LoadShaderModule(ResourceManager::GetVulkanShaderPath("Basic", ShaderStage::Compute).c_str(), m_device);
	VkShaderModule skyShader        = vkutil::LoadShaderModule(ResourceManager::GetVulkanShaderPath("Sky", ShaderStage::Compute).c_str(), m_device);
	VkShaderModule mandelbrotShader = vkutil::LoadShaderModule(ResourceManager::GetVulkanShaderPath("Mandelbrot", ShaderStage::Compute).c_str(), m_device);
	VkShaderModule waterShader      = vkutil::LoadShaderModule(ResourceManager::GetVulkanShaderPath("water", ShaderStage::Compute).c_str(), m_device);
	VkShaderModule raytracingShader = vkutil::LoadShaderModule(ResourceManager::GetVulkanShaderPath("Raytracing", ShaderStage::Compute).c_str(), m_device);

	VkPipelineShaderStageCreateInfo stageinfo{};
	stageinfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	stageinfo.pNext  = nullptr;
	stageinfo.stage  = VK_SHADER_STAGE_COMPUTE_BIT;
	stageinfo.module = gradientShader;
	stageinfo.pName  = "main";

	VkComputePipelineCreateInfo computePipelineCreateInfo{};
	computePipelineCreateInfo.sType  = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	computePipelineCreateInfo.pNext  = nullptr;
	computePipelineCreateInfo.layout = m_gradientPipelineLayout;
	computePipelineCreateInfo.stage  = stageinfo;

	ComputeEffect* gradient = CreateComputeEffect("gradient", gradientShader, m_gradientPipelineLayout, computePipelineCreateInfo);
	gradient->data.data1    = glm::vec4(1, 0, 0, 1);
	gradient->data.data2    = glm::vec4(0, 0, 1, 1);

	ComputeEffect* sky = CreateComputeEffect("sky", skyShader, m_gradientPipelineLayout, computePipelineCreateInfo);
	sky->data.data1    = glm::vec4(0.1, 0.2, 0.4, 0.97);

	CreateComputeEffect("mandelbrot", mandelbrotShader, m_gradientPipelineLayout, computePipelineCreateInfo);
	CreateComputeEffect("water", waterShader, m_gradientPipelineLayout, computePipelineCreateInfo);
	CreateComputeEffect("raytracing", raytracingShader, m_gradientPipelineLayout, computePipelineCreateInfo);

	m_mainDeletionQueue.push_function(
	    [this]()
	    {
		    for (auto effect : backgroundEffects)
		    {
			    vkDestroyPipeline(m_device, effect->pipeline, nullptr);
		    }
	    });
}

void VulkanEngine::InitMeshPipeline()
{
	VkShaderModule meshVertexShader = vkutil::LoadShaderModule(ResourceManager::GetVulkanShaderPath("Mesh", ShaderStage::Vertex).c_str(), m_device);
	VkShaderModule meshFragShader   = vkutil::LoadShaderModule(ResourceManager::GetVulkanShaderPath("Mesh", ShaderStage::Fragment).c_str(), m_device);

	// build the pipeline layout that controls the inputs/outputs of the shader
	// we are not using descriptor sets or other systems yet, so no need to use
	// anything other than empty default

	VkPushConstantRange matrixRange{};
	matrixRange.offset     = 0;
	matrixRange.size       = sizeof(GPUDrawPushConstants);
	matrixRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkDescriptorSetLayout layouts[] = { m_gpuSceneDataDescriptorLayout, m_gltfMatDescriptorLayout };

	VkPipelineLayoutCreateInfo mesh_layout_info = vkinit::PipelineLayoutCreateInfo();
	mesh_layout_info.setLayoutCount             = 2;
	mesh_layout_info.pSetLayouts                = layouts;
	mesh_layout_info.pPushConstantRanges        = &matrixRange;
	mesh_layout_info.pushConstantRangeCount     = 1;

	VkPipelineLayout defaultLayout;
	VK_CHECK(vkCreatePipelineLayout(m_device, &mesh_layout_info, nullptr, &defaultLayout));

	m_gltfDefaultOpaque.layout      = defaultLayout;
	m_gltfDefaultTranslucent.layout = defaultLayout;

	// build the stage-create-info for both vertex and fragment stages. This lets
	// the pipeline know the shader modules per stage
	vkutil::PipelineBuilder pipelineBuilder;
	pipelineBuilder.SetShaders(meshVertexShader, meshFragShader);
	pipelineBuilder.SetInputTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
	pipelineBuilder.SetPolygonMode(VK_POLYGON_MODE_FILL);
	pipelineBuilder.SetCullMode(VK_CULL_MODE_NONE, VK_FRONT_FACE_COUNTER_CLOCKWISE);
	pipelineBuilder.SetMultisamplingNone();
	pipelineBuilder.DisableBlending();
	pipelineBuilder.EnableDepthtest(true, VK_COMPARE_OP_LESS_OR_EQUAL);

	// render format
	pipelineBuilder.SetColorAttachmentFormat(m_drawImage.imageFormat);
	pipelineBuilder.SetDepthFormat(m_depthImage.imageFormat);

	// use the triangle layout we created
	pipelineBuilder.m_pipelineLayout = m_gltfDefaultOpaque.layout;

	// finally build the pipeline
	m_gltfDefaultOpaque.pipeline = pipelineBuilder.BuildPipeline(m_device);
	m_gltfDefaultOpaque.passType = MaterialPass::MainColor;

	// create the transparent variant
	pipelineBuilder.EnableBlendingAdditive();

	pipelineBuilder.EnableDepthtest(false, VK_COMPARE_OP_LESS_OR_EQUAL);

	m_gltfDefaultTranslucent.pipeline = pipelineBuilder.BuildPipeline(m_device);
	m_gltfDefaultTranslucent.passType = MaterialPass::Transparent;

	vkDestroyShaderModule(m_device, meshFragShader, nullptr);
	vkDestroyShaderModule(m_device, meshVertexShader, nullptr);

	m_mainDeletionQueue.push_function(
	    [&]()
	    {
		    vkDestroyPipelineLayout(m_device, m_gradientPipelineLayout, nullptr);
		    vkDestroyPipelineLayout(m_device, m_gltfDefaultOpaque.layout, nullptr);

		    vkDestroyPipeline(m_device, m_gltfDefaultTranslucent.pipeline, nullptr);
		    vkDestroyPipeline(m_device, m_gltfDefaultOpaque.pipeline, nullptr);
	    });
}

void VulkanEngine::InitImgui()
{
	// 1: create descriptor pool for IMGUI
	//  the size of the pool is very oversize, but it's copied from imgui demo
	//  itself.
	VkDescriptorPoolSize pool_sizes[] = { { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
		                                  { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
		                                  { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
		                                  { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
		                                  { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
		                                  { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
		                                  { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
		                                  { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
		                                  { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
		                                  { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
		                                  { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 } };

	VkDescriptorPoolCreateInfo pool_info = {};
	pool_info.sType                      = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.flags                      = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	pool_info.maxSets                    = 1000;
	pool_info.poolSizeCount              = (uint32_t)std::size(pool_sizes);
	pool_info.pPoolSizes                 = pool_sizes;

	VkDescriptorPool imguiPool;
	VK_CHECK(vkCreateDescriptorPool(m_device, &pool_info, nullptr, &imguiPool));

	// 2: initialize imgui library

	// this initializes the core structures of imgui
	ImGui::CreateContext();

	// this initializes imgui for SDL
	ImGui_ImplSDL3_InitForVulkan(m_window);

	// this initializes imgui for Vulkan
	ImGui_ImplVulkan_InitInfo init_info = {};
	init_info.Instance                  = m_instance;
	init_info.PhysicalDevice            = m_chosenGPU;
	init_info.Device                    = m_device;
	init_info.Queue                     = m_graphicsQueue;
	init_info.DescriptorPool            = imguiPool;
	init_info.MinImageCount             = 3;
	init_info.ImageCount                = 3;
	init_info.UseDynamicRendering       = true;
	init_info.ColorAttachmentFormat     = m_swapchainImageFormat;

	init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

	ImGui_ImplVulkan_Init(&init_info, VK_NULL_HANDLE);

	// execute a gpu command to upload imgui font textures
	ImmediateSubmit([](VkCommandBuffer cmd) { ImGui_ImplVulkan_CreateFontsTexture(); });

	// clear font textures from cpu data
	ImGui_ImplVulkan_DestroyFontsTexture();

	// add the destroy the imgui created structures
	m_mainDeletionQueue.push_function(
	    [=]()
	    {
		    ImGui_ImplVulkan_Shutdown();
		    vkDestroyDescriptorPool(m_device, imguiPool, nullptr);
	    });
}

AllocatedBuffer VulkanEngine::CreateBuffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage)
{
	// allocate buffer
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType              = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.pNext              = nullptr;
	bufferInfo.size               = allocSize;

	bufferInfo.usage = usage;

	VmaAllocationCreateInfo vmaallocInfo = {};
	vmaallocInfo.usage                   = memoryUsage;
	vmaallocInfo.flags                   = VMA_ALLOCATION_CREATE_MAPPED_BIT;
	AllocatedBuffer newBuffer;

	// allocate the buffer
	VK_CHECK(vmaCreateBuffer(m_allocator, &bufferInfo, &vmaallocInfo, &newBuffer.buffer, &newBuffer.allocation, &newBuffer.info));

	return newBuffer;
}

void VulkanEngine::DestroyBuffer(const AllocatedBuffer& buffer)
{
	vmaDestroyBuffer(m_allocator, buffer.buffer, buffer.allocation);
}

GPUMeshBuffers VulkanEngine::UploadMesh(std::span<uint32_t> indices, std::span<Vertex> vertices)
{
	const size_t vertexBufferSize = vertices.size() * sizeof(Vertex);
	const size_t indexBufferSize  = indices.size() * sizeof(uint32_t);

	GPUMeshBuffers newSurface;

	// create vertex buffer
	newSurface.vertexBuffer = CreateBuffer(vertexBufferSize,
	                                       VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
	                                       VMA_MEMORY_USAGE_GPU_ONLY);

	// find the adress of the vertex buffer
	VkBufferDeviceAddressInfo deviceAdressInfo{ .sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO, .buffer = newSurface.vertexBuffer.buffer };
	newSurface.vertexBufferAddress = vkGetBufferDeviceAddress(m_device, &deviceAdressInfo);

	// create index buffer
	newSurface.indexBuffer = CreateBuffer(indexBufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_ONLY);

	AllocatedBuffer staging = CreateBuffer(vertexBufferSize + indexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

	void* data = staging.allocation->GetMappedData();

	// Copy the vertex data
	memcpy(data, vertices.data(), vertexBufferSize);

	// Copy the index data
	memcpy((void*)((size_t)data + vertexBufferSize), indices.data(), indexBufferSize);

	ImmediateSubmit(
	    [=](VkCommandBuffer cmd)
	    {
		    VkBufferCopy vertexCopy{ 0 };
		    vertexCopy.dstOffset = 0;
		    vertexCopy.srcOffset = 0;
		    vertexCopy.size      = vertexBufferSize;

		    vkCmdCopyBuffer(cmd, staging.buffer, newSurface.vertexBuffer.buffer, 1, &vertexCopy);

		    VkBufferCopy indexCopy{ 0 };
		    indexCopy.dstOffset = 0;
		    indexCopy.srcOffset = vertexBufferSize;
		    indexCopy.size      = indexBufferSize;

		    vkCmdCopyBuffer(cmd, staging.buffer, newSurface.indexBuffer.buffer, 1, &indexCopy);
	    });

	DestroyBuffer(staging);

	return newSurface;
}

void VulkanEngine::InitDefaultData()
{
	std::string structurePath = ResourceManager::GetModelPath("structure", ".glb");
	auto structureFile        = LoadGltf(structurePath);
	assert(structureFile.has_value());
	m_loadedScenes["structure"] = *structureFile;


	std::string monkeyPath = ResourceManager::GetModelPath("monkey2", ".glb");
	auto monkeyFile     = LoadGltf(monkeyPath);
	assert(monkeyFile.has_value());
	m_loadedScenes["monkey"] = *monkeyFile;
}

void VulkanEngine::Update()
{
	SDL_Event e;
	bool running            = true;
	static bool skipDrawing = false;

	// main loop
	while (running)
	{
		auto start = std::chrono::system_clock::now();
		// Handle events on queue
		while (SDL_PollEvent(&e) != 0)
		{
			// close the window when user clicks the X button or alt-f4s
			if (e.type == SDL_EVENT_QUIT)
				running = false;

			// If the user presses esc
			if (e.type == SDL_EVENT_KEY_DOWN)
			{
				if (e.key.keysym.sym == SDLK_ESCAPE)
				{
					running = false;
				}
			}
			if (e.type == SDL_EVENT_WINDOW_MINIMIZED)
			{
				skipDrawing = true;
			}
			if (e.type == SDL_EVENT_WINDOW_RESTORED)
			{
				skipDrawing = false;
			}
			if (e.type == SDL_EVENT_WINDOW_RESIZED)
			{
				InitSwapchain();
			}

			mainCamera.ProcessSDLEvent(e);

			// send SDL event to imgui for handling
			ImGui_ImplSDL3_ProcessEvent(&e);
		}

		// imgui new frame
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplSDL3_NewFrame();
		ImGui::NewFrame();
		if (ImGui::Begin("Stats", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
		{
			// Set the window to be at the top left of the screen
			ImGui::SetWindowSize(ImVec2(200.0f, 100.0f));
			ImVec2 windowPos = ImVec2(0.0f, 0.0f);
			ImGui::SetWindowPos(windowPos);

			ImGui::Text("frametime %f ms", m_stats.frameTime);
			ImGui::Text("drawtime %f ms", m_stats.meshDrawTime);
			ImGui::Text("triangles %i", m_stats.triangleCount);
			ImGui::Text("draws %i", m_stats.drawcallCount);
			ImGui::End();
		}
		if (ImGui::Begin("background", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
		{
			// Set the window position to the bottom right corner
			ImGui::SetWindowSize(ImVec2(300.0f, 150.0f));
			ImVec2 windowPos = ImVec2(ImGui::GetIO().DisplaySize.x - ImGui::GetWindowWidth(), ImGui::GetIO().DisplaySize.y - ImGui::GetWindowHeight());
			ImGui::SetWindowPos(windowPos);

			ComputeEffect* selected = backgroundEffects[currentBackgroundEffect];

			ImGui::Text("Selected effect: ", selected->name);

			ImGui::SliderInt("Effect Index", &currentBackgroundEffect, 0, backgroundEffects.size() - 1);

			ImGui::SliderFloat4("data1", (float*)&selected->data.data1, 0.0f, 1.0f);
			ImGui::SliderFloat4("data2", (float*)&selected->data.data2, 0.0f, 1.0f);
			ImGui::SliderFloat4("data3", (float*)&selected->data.data3, 0.0f, 1.0f);
			// ImGui::SliderFloat4("data4", (float*)&selected.data.data4, 0.0f, 1.0f); // This will be adjusted automatically in this update

			// update data4
			float time             = SDL_GetTicks() / 1000.0f;
			selected->data.data4.x = time;

			// Mouse pos is y and z
			selected->data.data4.y = ImGui::GetIO().MousePos.x;
			selected->data.data4.z = ImGui::GetIO().MousePos.y;

			ImGui::End();
		}

		// make imgui calculate internal draw structures
		ImGui::Render();

		mainCamera.Update();

		glm::mat4 view = mainCamera.GetViewMatrix();

		// camera projection
		glm::mat4 projection = mainCamera.CalculateProjectionMatrix(16.0f / 9.0f);

		// invert the Y direction on projection matrix so that we are more similar
		// to opengl and gltf axis
		projection[1][1] *= -1;

		m_sceneData.view     = view;
		m_sceneData.proj     = projection;
		m_sceneData.viewproj = projection * view;
		m_sceneData.sunlightColor = glm::vec4(1.0f, 0.6f, 0.8f, 10.0f);
		m_sceneData.ambientColor = glm::vec4(0.2f, 0.2f, 0.2f, 0.1f);
		m_sceneData.sunlightDirection = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);

		m_loadedScenes["structure"]->Draw(glm::mat4{ 1.f }, m_drawCommands);
		m_loadedScenes["monkey"]->Draw(glm::mat4{ 1.f }, m_drawCommands);

		if (!skipDrawing)
		{
			Draw();
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds{ 100 });
		}

		auto end     = std::chrono::system_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

		m_stats.frameTime = elapsed.count() / 1000.f;
	}
}

void VulkanEngine::Draw()
{
	// wait until the gpu has finished rendering the last frame. Timeout of 1 second
	VK_CHECK(vkWaitForFences(m_device, 1, &GetCurrentFrame().m_renderFence, true, 1000000000));

	GetCurrentFrame().m_deletionQueue.flush();
	GetCurrentFrame().m_frameDescriptors.ClearDescriptors(m_device);
	// request image from the swapchain
	uint32_t swapchainImageIndex;

	VkResult e = vkAcquireNextImageKHR(m_device, m_swapchain, 1000000000, GetCurrentFrame().m_swapchainSemaphore, nullptr, &swapchainImageIndex);
	if (e == VK_ERROR_OUT_OF_DATE_KHR)
	{
		return;
	}

	VK_CHECK(vkResetFences(m_device, 1, &GetCurrentFrame().m_renderFence));

	// now that we are sure that the commands finished executing, we can safely reset the command buffer to begin recording again.
	VK_CHECK(vkResetCommandBuffer(GetCurrentFrame().m_mainCommandBuffer, 0));

	// naming it cmd for shorter writing
	VkCommandBuffer cmd = GetCurrentFrame().m_mainCommandBuffer;

	// begin the command buffer recording. We will use this command buffer exactly once, so we want to let vulkan know that
	VkCommandBufferBeginInfo cmdBeginInfo = vkinit::CommandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	//> draw_first
	VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

	// transition our main draw image into general layout so we can write into it
	// we will overwrite it all so we dont care about what was the older layout
	vkutil::TransitionImage(cmd, m_drawImage.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

	DrawMain(cmd);

	// transtion the draw image and the swapchain image into their correct transfer layouts
	vkutil::TransitionImage(cmd, m_drawImage.image, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
	vkutil::TransitionImage(cmd, m_swapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	VkExtent3D extent;
	extent.height = m_windowExtent.height;
	extent.width  = m_windowExtent.width;
	extent.depth  = 1;
	//< draw_first
	//> imgui_draw
	// execute a copy from the draw image into the swapchain
	vkutil::CopyImageToImage(cmd, m_drawImage.image, m_swapchainImages[swapchainImageIndex], extent);

	// set swapchain image layout to Attachment Optimal so we can draw it
	vkutil::TransitionImage(cmd, m_swapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

	// draw imgui into the swapchain image
	DrawImgui(cmd, m_swapchainImageViews[swapchainImageIndex]);

	// set swapchain image layout to Present so we can draw it
	vkutil::TransitionImage(cmd, m_swapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

	// finalize the command buffer (we can no longer add commands, but it can now be executed)
	VK_CHECK(vkEndCommandBuffer(cmd));

	// prepare the submission to the queue.
	// we want to wait on the _presentSemaphore, as that semaphore is signaled when the swapchain is ready
	// we will signal the _renderSemaphore, to signal that rendering has finished

	VkCommandBufferSubmitInfo cmdinfo = vkinit::CommandBufferSubmitInfo(cmd);

	VkSemaphoreSubmitInfo waitInfo   = vkinit::SemaphoreSubmitInfo(VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR, GetCurrentFrame().m_swapchainSemaphore);
	VkSemaphoreSubmitInfo signalInfo = vkinit::SemaphoreSubmitInfo(VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT, GetCurrentFrame().m_renderSemaphore);

	VkSubmitInfo2 submit = vkinit::SubmitInfo(&cmdinfo, &signalInfo, &waitInfo);

	// submit command buffer to the queue and execute it.
	//  m_renderFence will now block until the graphic commands finish execution
	VK_CHECK(vkQueueSubmit2(m_graphicsQueue, 1, &submit, GetCurrentFrame().m_renderFence));

	// prepare present
	//  this will put the image we just rendered to into the visible window.
	//  we want to wait on the _renderSemaphore for that,
	//  as its necessary that drawing commands have finished before the image is displayed to the user
	VkPresentInfoKHR presentInfo = vkinit::PresentInfo();

	presentInfo.pSwapchains    = &m_swapchain;
	presentInfo.swapchainCount = 1;

	presentInfo.pWaitSemaphores    = &GetCurrentFrame().m_renderSemaphore;
	presentInfo.waitSemaphoreCount = 1;

	presentInfo.pImageIndices = &swapchainImageIndex;

	VkResult presentResult = vkQueuePresentKHR(m_graphicsQueue, &presentInfo);

	// increase the number of frames drawn
	m_frameNumber++;
}

void VulkanEngine::DrawMain(VkCommandBuffer cmd)
{
	// DRAW BACKGROUND
	ComputeEffect* effect = backgroundEffects[currentBackgroundEffect];

	// Bind the background compute pipeline
	vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, effect->pipeline);

	// Bind the descriptor set containing the draw image for the compute pipeline
	vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, m_gradientPipelineLayout, 0, 1, &m_drawImageDescriptors, 0, nullptr);

	vkCmdPushConstants(cmd, m_gradientPipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(ComputePushConstants), &effect->data);

	// Execute the compute pipeline dispatch. We are using a 16x16 workgroup size, so we need to divide by it
	vkCmdDispatch(cmd, static_cast<uint32_t>(std::ceil(m_windowExtent.width / 16.0)), static_cast<uint32_t>(std::ceil(m_windowExtent.height / 16.0)), 1);

	// DRAW GEOMETRY
	VkRenderingAttachmentInfo colorAttachment = vkinit::AttachmentInfo(m_drawImage.imageView, nullptr, VK_IMAGE_LAYOUT_GENERAL);
	VkRenderingAttachmentInfo depthAttachment = vkinit::DepthAttachmentInfo(m_depthImage.imageView, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);

	VkRenderingInfo renderInfo = vkinit::RenderingInfo(m_windowExtent, &colorAttachment, &depthAttachment);

	// Begin the render pass for both depth pass and color pass
	vkCmdBeginRendering(cmd, &renderInfo);

	// Color pass (Geometry)
	auto start = std::chrono::system_clock::now();
	DrawGeometry(cmd);
	auto end     = std::chrono::system_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

	m_stats.meshDrawTime = elapsed.count() / 1000.f;

	// End the render pass
	vkCmdEndRendering(cmd);
}


void VulkanEngine::DrawImgui(VkCommandBuffer cmd, VkImageView targetImageView)
{
	VkRenderingAttachmentInfo colorAttachment = vkinit::AttachmentInfo(targetImageView, nullptr, VK_IMAGE_LAYOUT_GENERAL);
	VkRenderingInfo renderInfo                = vkinit::RenderingInfo(m_windowExtent, &colorAttachment, nullptr);

	vkCmdBeginRendering(cmd, &renderInfo);

	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);

	vkCmdEndRendering(cmd);
}

void VulkanEngine::DrawGeometry(VkCommandBuffer cmd)
{
	// sort the opaque surfaces by material and mesh
	std::sort(m_drawCommands.OpaqueSurfaces.begin(),
	          m_drawCommands.OpaqueSurfaces.end(),
	          [](const auto& A, const auto& B)
	          {
		          if (A.material == B.material)
		          {
			          return A.indexBuffer < B.indexBuffer;
		          }

		          return A.material < B.material;
	          });

	GPUSceneData* sceneUniformData = (GPUSceneData*)GetCurrentFrame().cameraBuffer.allocation->GetMappedData();
	*sceneUniformData              = m_sceneData;

	VkDescriptorSet globalDescriptor = GetCurrentFrame().m_frameDescriptors.Allocate(m_device, m_gpuSceneDataDescriptorLayout);

	DescriptorWriter writer;
	writer.WriteBuffer(0, GetCurrentFrame().cameraBuffer.buffer, sizeof(GPUSceneData), 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
	writer.Build(m_device, globalDescriptor);

	VkPipeline lastPipeline    = VK_NULL_HANDLE;
	MaterialData* lastMaterial = nullptr;
	VkBuffer lastIndexBuffer   = VK_NULL_HANDLE;

	auto draw = [&](const RenderObject& r)
	{
		if (r.material != lastMaterial)
		{
			lastMaterial = r.material;
			if (r.material->pipeline != lastPipeline)
			{

				lastPipeline = r.material->pipeline;
				vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, r.material->pipeline);
				vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, r.material->layout, 0, 1, &globalDescriptor, 0, nullptr);

				VkViewport viewport = {};
				viewport.x          = 0;
				viewport.y          = 0;
				viewport.width      = (float)m_windowExtent.width;
				viewport.height     = (float)m_windowExtent.height;
				viewport.minDepth   = 0.f;
				viewport.maxDepth   = 1.f;

				vkCmdSetViewport(cmd, 0, 1, &viewport);

				VkRect2D scissor      = {};
				scissor.offset.x      = 0;
				scissor.offset.y      = 0;
				scissor.extent.width  = m_windowExtent.width;
				scissor.extent.height = m_windowExtent.height;

				vkCmdSetScissor(cmd, 0, 1, &scissor);
			}

			vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, r.material->layout, 1, 1, &r.material->materialSet, 0, nullptr);
		}
		if (r.indexBuffer != lastIndexBuffer)
		{
			lastIndexBuffer = r.indexBuffer;
			vkCmdBindIndexBuffer(cmd, r.indexBuffer, 0, VK_INDEX_TYPE_UINT32);
		}
		// calculate final mesh matrix
		GPUDrawPushConstants push_constants;
		push_constants.worldMatrix  = r.transform;
		push_constants.vertexBuffer = r.vertexBufferAddress;

		vkCmdPushConstants(cmd, r.material->layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(GPUDrawPushConstants), &push_constants);

		m_stats.drawcallCount++;
		m_stats.triangleCount += r.indexCount / 3;
		vkCmdDrawIndexed(cmd, r.indexCount, 1, r.firstIndex, 0, 0);
	};

	m_stats.drawcallCount = 0;
	m_stats.triangleCount = 0;

	for (auto& r : m_drawCommands.OpaqueSurfaces)
	{
		draw(r);
	}

	for (auto& r : m_drawCommands.TransparentSurfaces)
	{
		draw(r);
	}

	// we delete the draw commands now that we processed them
	m_drawCommands.OpaqueSurfaces.clear();
	m_drawCommands.TransparentSurfaces.clear();
}

void VulkanEngine::Cleanup()
{
	if (m_isInitialized)
	{
		if (m_isInitialized)
		{

			// make sure the gpu has stopped doing its things
			vkDeviceWaitIdle(m_device);

			m_loadedScenes.clear();

			for (auto& frame : m_frames)
			{
				frame.m_deletionQueue.flush();
			}

			m_mainDeletionQueue.flush();

			vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);

			// destroy swapchain resources
			for (int i = 0; i < m_swapchainImageViews.size(); i++)
			{
				vkDestroyImageView(m_device, m_swapchainImageViews[i], nullptr);
			}

			vkDestroySurfaceKHR(m_instance, m_surface, nullptr);

			vmaDestroyAllocator(m_allocator);

			vkDestroyDevice(m_device, nullptr);
			vkb::destroy_debug_utils_messenger(m_instance, m_debug_messenger);
			vkDestroyInstance(m_instance, nullptr);

			SDL_DestroyWindow(m_window);
		}
	}
}

void VulkanEngine::ImmediateSubmit(std::function<void(VkCommandBuffer cmd)>&& function)
{
	VK_CHECK(vkResetFences(m_device, 1, &m_immFence));
	VK_CHECK(vkResetCommandBuffer(m_immCommandBuffer, 0));

	VkCommandBuffer cmd = m_immCommandBuffer;

	VkCommandBufferBeginInfo cmdBeginInfo = vkinit::CommandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

	function(cmd);

	VK_CHECK(vkEndCommandBuffer(cmd));

	VkCommandBufferSubmitInfo cmdinfo = vkinit::CommandBufferSubmitInfo(cmd);
	VkSubmitInfo2 submit              = vkinit::SubmitInfo(&cmdinfo, nullptr, nullptr);

	// submit command buffer to the queue and execute it.
	//  m_renderFence will now block until the graphic commands finish execution
	VK_CHECK(vkQueueSubmit2(m_graphicsQueue, 1, &submit, m_immFence));

	VK_CHECK(vkWaitForFences(m_device, 1, &m_immFence, true, 9999999999));
}
