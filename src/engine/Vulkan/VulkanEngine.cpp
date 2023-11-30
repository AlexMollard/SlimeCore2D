#include "VulkanEngine.h"

#include "VkBootstrap.h"

#define VMA_IMPLEMENTATION
#include "engine/ResourceManager.h"
#include "Pipeline.h"
#include "VulkanImages.h"
#include "VulkanInit.h"
#include "VulkanTypes.h"
#include "imgui/backends/imgui_impl_sdl3.h"
#include "imgui/backends/imgui_impl_vulkan.h"
#include "imgui/imgui.h"
#include "vulkan/vk_mem_alloc.h"
#include <SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <regex>
#include <sstream>

#include "engine/ConsoleLog.h"
#include "engine/MemoryDebugging.h"

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
}

void VulkanEngine::RebuildSwapchain()
{
	vkQueueWaitIdle(m_graphicsQueue);

	vkb::SwapchainBuilder swapchainBuilder{ m_chosenGPU, m_device, m_surface };

	SDL_GetWindowSizeInPixels(m_window, (int*)&m_windowExtent.width, (int*)&m_windowExtent.height);

	vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
	m_drawImage.Destroy(m_device, m_allocator);

	vkb::Swapchain vkbSwapchain =
	    swapchainBuilder
	        .set_desired_format(VkSurfaceFormatKHR{ .format = VK_FORMAT_B8G8R8A8_UNORM, .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR }) // Set the desired format
	        .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)                                                                            // use vsync present mode
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

	// hardcoding the depth format to 32 bit float
	m_drawImage.imageFormat = VK_FORMAT_R16G16B16A16_SFLOAT;

	VkImageUsageFlags drawImageUsages{};
	drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	drawImageUsages |= VK_IMAGE_USAGE_STORAGE_BIT;

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

	VkDescriptorImageInfo imgInfo{};
	imgInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
	imgInfo.imageView   = m_drawImage.imageView;

	VkWriteDescriptorSet cameraWrite = vkinit::WriteDescriptorImage(VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, m_mainDescriptorSet, &imgInfo, 0);

	vkUpdateDescriptorSets(m_device, 1, &cameraWrite, 0, nullptr);
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

		//if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
		//	abort();

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

	VkPhysicalDeviceVulkan13Features features{};
	features.dynamicRendering = true;
	features.synchronization2 = true;

	// use vkbootstrap to select a gpu.
	// We want a gpu that can write to the SDL surface and supports vulkan 1.3
	vkb::PhysicalDeviceSelector selector{ vkbInst };
	vkb::PhysicalDevice physicalDevice = selector
	                                         .set_minimum_version(1, 3)          // require Vulkan 1.3.0 or higher
	                                         .set_required_features_13(features) // require Vulkan 1.3.0 or higher
	                                         .set_surface(m_surface)             // Set the current surface as this is needed to determine what the gpu requires
	                                         .select()                           // Select the best gpu
	                                         .value();                           // Get the selected gpu

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
	vmaCreateAllocator(&allocatorInfo, &m_allocator);
}

void VulkanEngine::InitSwapchain()
{
	vkb::SwapchainBuilder swapchainBuilder{ m_chosenGPU, m_device, m_surface };

	vkb::Swapchain vkbSwapchain =
	    swapchainBuilder
	        .set_desired_format(VkSurfaceFormatKHR{ .format = VK_FORMAT_B8G8R8A8_UNORM, .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR }) // Set the desired format
	        .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)                                                                            // use vsync present mode
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

	// hardcoding the depth format to 32 bit float
	m_drawImage.imageFormat = VK_FORMAT_R16G16B16A16_SFLOAT;

	VkImageUsageFlags drawImageUsages{};
	drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	drawImageUsages |= VK_IMAGE_USAGE_STORAGE_BIT;

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
	VkFenceCreateInfo fenceCreateInfo         = vkinit::FenceCreateInfo(VK_FENCE_CREATE_SIGNALED_BIT);
	VkSemaphoreCreateInfo semaphoreCreateInfo = vkinit::SemaphoreCreateInfo();

	for (int i = 0; i < FRAME_OVERLAP; i++)
	{
		VK_CHECK(vkCreateFence(m_device, &fenceCreateInfo, nullptr, &m_frames[i].m_renderFence));

		VK_CHECK(vkCreateSemaphore(m_device, &semaphoreCreateInfo, nullptr, &m_frames[i].m_swapchainSemaphore));
		VK_CHECK(vkCreateSemaphore(m_device, &semaphoreCreateInfo, nullptr, &m_frames[i].m_renderSemaphore));
	}

	VK_CHECK(vkCreateFence(m_device, &fenceCreateInfo, nullptr, &m_immFence));
	m_mainDeletionQueue.push_function([=]() { vkDestroyFence(m_device, m_immFence, nullptr); });
}

void VulkanEngine::InitDescriptors()
{
	// create a descriptor pool that will hold 10 sets with 1 image each
	std::vector<DescriptorAllocator::PoolSizeRatio> sizes = { { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1 } };

	globalDescriptorAllocator.InitPool(m_device, 10, sizes);

	// make the descriptor set layout for our compute draw
	{
		DescriptorLayoutBuilder builder;
		builder.AddBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT);
		m_mainDescriptorLayout = builder.Build(m_device);

		// Create m_drawImageDescriptors
		m_mainDescriptorSet = globalDescriptorAllocator.Allocate(m_device, m_mainDescriptorLayout);

		globalDescriptorAllocator.UpdateImageDescriptor(m_device, m_mainDescriptorSet, m_drawImage.imageView, VK_IMAGE_LAYOUT_GENERAL, 0);
	}

	// add descriptor set layout to deletion queues
	m_mainDeletionQueue.push_function(
	    [&]()
	    {
		    vkDestroyDescriptorSetLayout(m_device, m_mainDescriptorLayout, nullptr);
		    globalDescriptorAllocator.DestroyPool(m_device);
	    });
}

void VulkanEngine::InitPipelines()
{
	VkPipelineLayoutCreateInfo computeLayout{};
	computeLayout.sType          = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	computeLayout.pNext          = nullptr;
	computeLayout.pSetLayouts    = &m_mainDescriptorLayout;
	computeLayout.setLayoutCount = 1;

	VkPushConstantRange pushConstant{};
	pushConstant.offset     = 0;
	pushConstant.size       = sizeof(ComputePushConstants);
	pushConstant.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

	computeLayout.pPushConstantRanges    = &pushConstant;
	computeLayout.pushConstantRangeCount = 1;

	VK_CHECK(vkCreatePipelineLayout(m_device, &computeLayout, nullptr, &m_gradientPipelineLayout));


	VkShaderModule gradientShader;
	if (!vkutil::LoadShaderModule(ResourceManager::GetVulkanShaderPath("Basic", ShaderStage::Compute).c_str(), m_device, &gradientShader))
	{
		SLIME_ERROR("Error when building the compute shader");
	}

	VkShaderModule skyShader;
	if (!vkutil::LoadShaderModule(ResourceManager::GetVulkanShaderPath("Sky", ShaderStage::Compute).c_str(), m_device, &skyShader))
	{
		SLIME_ERROR("Error when building the compute shader");
	}

	VkShaderModule mandelbrotShader;
	if (!vkutil::LoadShaderModule(ResourceManager::GetVulkanShaderPath("Mandelbrot", ShaderStage::Compute).c_str(), m_device, &mandelbrotShader))
	{
		SLIME_ERROR("Error when building the compute shader");
	}

	VkShaderModule waterShader;
	if (!vkutil::LoadShaderModule(ResourceManager::GetVulkanShaderPath("water", ShaderStage::Compute).c_str(), m_device, &waterShader))
	{
		SLIME_ERROR("Error when building the compute shader");
	}

	VkShaderModule cubeShader;
	if (!vkutil::LoadShaderModule(ResourceManager::GetVulkanShaderPath("CubeLines", ShaderStage::Compute).c_str(), m_device, &cubeShader))
	{
		SLIME_ERROR("Error when building the compute shader");
	}

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

	ComputeEffect gradient;
	gradient.layout = m_gradientPipelineLayout;
	gradient.name   = "gradient";
	gradient.data   = {};

	// default colors
	gradient.data.data1 = glm::vec4(1, 0, 0, 1);
	gradient.data.data2 = glm::vec4(0, 0, 1, 1);

	VK_CHECK(vkCreateComputePipelines(m_device, VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr, &gradient.pipeline));

	// change the shader module only to create the sky shader
	computePipelineCreateInfo.stage.module = skyShader;

	ComputeEffect sky;
	sky.layout = m_gradientPipelineLayout;
	sky.name   = "sky";
	sky.data   = {};
	// default sky parameters
	sky.data.data1 = glm::vec4(0.1, 0.2, 0.4, 0.97);

	VK_CHECK(vkCreateComputePipelines(m_device, VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr, &sky.pipeline));

	computePipelineCreateInfo.stage.module = mandelbrotShader;
	ComputeEffect mandelbrot;
	mandelbrot.layout = m_gradientPipelineLayout;
	mandelbrot.name   = "mandelbrot";
	mandelbrot.data   = {};
	// default sky parameters
	mandelbrot.data.data1 = glm::vec4(0.1, 0.2, 0.4, 0.97);

	VK_CHECK(vkCreateComputePipelines(m_device, VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr, &mandelbrot.pipeline));

	computePipelineCreateInfo.stage.module = waterShader;
	ComputeEffect water;
	water.layout      = m_gradientPipelineLayout;
	water.name      = "water";
	water.data        = {};
	// default sky parameters
	water.data.data1 = glm::vec4(0.1, 0.2, 0.4, 0.97);

	VK_CHECK(vkCreateComputePipelines(m_device, VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr, &water.pipeline));

	computePipelineCreateInfo.stage.module = cubeShader;
	ComputeEffect cube;
	cube.layout  = m_gradientPipelineLayout;
	cube.name   = "cube";
	cube.data    = {};
	// default sky parameters
	cube.data.data1 = glm::vec4(0.1, 0.2, 0.4, 0.97);

	VK_CHECK(vkCreateComputePipelines(m_device, VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr, &cube.pipeline));

	backgroundEffects.push_back(gradient);
	backgroundEffects.push_back(sky);
	backgroundEffects.push_back(mandelbrot);
	backgroundEffects.push_back(water);
	backgroundEffects.push_back(cube);

	// destroy structures properly
	vkDestroyShaderModule(m_device, gradientShader, nullptr);
	vkDestroyShaderModule(m_device, skyShader, nullptr);
	vkDestroyShaderModule(m_device, mandelbrotShader, nullptr);
	vkDestroyShaderModule(m_device, waterShader, nullptr);
	vkDestroyShaderModule(m_device, cubeShader, nullptr);
	m_mainDeletionQueue.push_function(
	    [&]()
	    {
		    vkDestroyPipelineLayout(m_device, m_gradientPipelineLayout, nullptr);
		    vkDestroyPipeline(m_device, cube.pipeline, nullptr);
		    vkDestroyPipeline(m_device, water.pipeline, nullptr);
		    vkDestroyPipeline(m_device, mandelbrot.pipeline, nullptr);
		    vkDestroyPipeline(m_device, sky.pipeline, nullptr);
		    vkDestroyPipeline(m_device, gradient.pipeline, nullptr);
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

void VulkanEngine::Update()
{
	SDL_Event e;
	bool running            = true;
	static bool skipDrawing = false;

	// main loop
	while (running)
	{
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
				RebuildSwapchain();
			}

			// send SDL event to imgui for handling
			ImGui_ImplSDL3_ProcessEvent(&e);
		}

		// imgui new frame
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplSDL3_NewFrame();
		ImGui::NewFrame();

		if (ImGui::Begin("background", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
		{
			// Set the window position to the bottom right corner
			ImGui::SetWindowSize(ImVec2(300.0f, 150.0f));
			ImVec2 windowPos = ImVec2(ImGui::GetIO().DisplaySize.x - ImGui::GetWindowWidth(), ImGui::GetIO().DisplaySize.y - ImGui::GetWindowHeight());
			ImGui::SetWindowPos(windowPos);

			ComputeEffect& selected = backgroundEffects[currentBackgroundEffect];

			ImGui::Text("Selected effect: ", selected.name);

			ImGui::SliderInt("Effect Index", &currentBackgroundEffect, 0, backgroundEffects.size() - 1);

			ImGui::SliderFloat4("data1", (float*)&selected.data.data1, 0.0f, 1.0f);
			ImGui::SliderFloat4("data2", (float*)&selected.data.data2, 0.0f, 1.0f);
			ImGui::SliderFloat4("data3", (float*)&selected.data.data3, 0.0f, 1.0f);
			// ImGui::SliderFloat4("data4", (float*)&selected.data.data4, 0.0f, 1.0f); // This will be adjusted automatically in this update
			
			// update data4
			float time = SDL_GetTicks() / 1000.0f;
			selected.data.data4.x = time;
			selected.data.data4.y = time * 2.0f;

			ImGui::End();
		}

		// make imgui calculate internal draw structures
		ImGui::Render();


		if (!skipDrawing)
		{
			Draw();
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds{ 100 });
		}
	}
}

void VulkanEngine::Draw()
{
	VK_CHECK(vkWaitForFences(m_device, 1, &GetCurrentFrame().m_renderFence, true, ONE_SECOND));

	GetCurrentFrame().m_deletionQueue.flush();

	// request image from the swapchain
	uint32_t swapchainImageIndex;

	VkResult e = vkAcquireNextImageKHR(m_device, m_swapchain, ONE_SECOND, GetCurrentFrame().m_swapchainSemaphore, nullptr, &swapchainImageIndex);
	if (e == VK_ERROR_OUT_OF_DATE_KHR)
	{
		RebuildSwapchain();
		return;
	}

	VK_CHECK(vkResetFences(m_device, 1, &GetCurrentFrame().m_renderFence));

	// now that we are sure that the commands finished executing, we can safely reset the command buffer to begin recording again.
	VK_CHECK(vkResetCommandBuffer(GetCurrentFrame().m_mainCommandBuffer, 0));

	// naming it cmd for shorter writing
	VkCommandBuffer cmd = GetCurrentFrame().m_mainCommandBuffer;

	// begin the command buffer recording. We will use this command buffer exactly once, so we want to let vulkan know that
	VkCommandBufferBeginInfo cmdBeginInfo = vkinit::CommandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	// start the command buffer recording
	VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

	// transition our main draw image into general layout so we can write into it
	// we will overwrite it all so we dont care about what was the older layout
	vkutil::TransitionImage(cmd, m_drawImage.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

	Render(cmd);

	// transtion the draw image and the swapchain image into their correct transfer layouts
	vkutil::TransitionImage(cmd, m_drawImage.image, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
	vkutil::TransitionImage(cmd, m_swapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	VkExtent3D extent;
	extent.height = m_windowExtent.height;
	extent.width  = m_windowExtent.width;
	extent.depth  = 1;

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
	// we want to wait on the m_presentSemaphore, as that semaphore is signaled when the swapchain is ready
	// we will signal the m_renderSemaphore, to signal that rendering has finished
	VkCommandBufferSubmitInfo cmdinfo = vkinit::CommandBufferSubmitInfo(cmd);

	VkSemaphoreSubmitInfo waitInfo   = vkinit::SemaphoreSubmitInfo(VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR, GetCurrentFrame().m_swapchainSemaphore);
	VkSemaphoreSubmitInfo signalInfo = vkinit::SemaphoreSubmitInfo(VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT, GetCurrentFrame().m_renderSemaphore);

	VkSubmitInfo2 submit = vkinit::SubmitInfo(&cmdinfo, &signalInfo, &waitInfo);

	// submit command buffer to the queue and execute it.
	//  _renderFence will now block until the graphic commands finish execution
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

void VulkanEngine::Render(VkCommandBuffer cmd)
{
	ComputeEffect& effect = backgroundEffects[currentBackgroundEffect];

	// bind the background compute pipeline
	vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, effect.pipeline);

	// bind the descriptor set containing the draw image for the compute pipeline
	vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, m_gradientPipelineLayout, 0, 1, &m_mainDescriptorSet, 0, nullptr);

	vkCmdPushConstants(cmd, m_gradientPipelineLayout, VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(ComputePushConstants), &effect.data);
	// execute the compute pipeline dispatch. We are using 16x16 workgroup size so we need to divide by it
	vkCmdDispatch(cmd, std::ceil(m_windowExtent.width / 16.0), std::ceil(m_windowExtent.height / 16.0), 1);
}

void VulkanEngine::DrawImgui(VkCommandBuffer cmd, VkImageView targetImageView)
{
	VkRenderingAttachmentInfo colorAttachment = vkinit::AttachmentInfo(targetImageView, nullptr, VK_IMAGE_LAYOUT_GENERAL);
	VkRenderingInfo renderInfo                = vkinit::RenderingInfo(m_windowExtent, &colorAttachment, nullptr);

	vkCmdBeginRendering(cmd, &renderInfo);

	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);

	vkCmdEndRendering(cmd);
}

void VulkanEngine::Cleanup()
{
	if (m_isInitialized)
	{
		if (m_isInitialized)
		{

			// make sure the gpu has stopped doing its things
			vkDeviceWaitIdle(m_device);
			m_mainDeletionQueue.flush();

			for (auto& frame : m_frames)
			{
				vkDestroyCommandPool(m_device, frame.m_commandPool, nullptr);

				// destroy sync objects
				vkDestroyFence(m_device, frame.m_renderFence, nullptr);
				vkDestroySemaphore(m_device, frame.m_renderSemaphore, nullptr);
				vkDestroySemaphore(m_device, frame.m_swapchainSemaphore, nullptr);

				frame.m_deletionQueue.flush();
			}

			vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);

			// destroy swapchain resources
			for (int i = 0; i < m_swapchainImageViews.size(); i++)
			{
				vkDestroyImageView(m_device, m_swapchainImageViews[i], nullptr);
			}

			m_drawImage.Destroy(m_device, m_allocator);

			vmaDestroyAllocator(m_allocator);
			vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
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
	//  _renderFence will now block until the graphic commands finish execution
	VK_CHECK(vkQueueSubmit2(m_graphicsQueue, 1, &submit, m_immFence));

	VK_CHECK(vkWaitForFences(m_device, 1, &m_immFence, true, ONE_SECOND));
}
