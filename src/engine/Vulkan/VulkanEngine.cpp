#include "VulkanEngine.h"

#include "VulkanImages.h"
#include "VulkanInit.h"
#include "VulkanTypes.h"
#include <SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <regex>

#include "engine/ConsoleLog.h"
#include "engine/MemoryDebugging.h"

constexpr uint64_t ONE_SECOND = 1000000000;

void VulkanEngine::Init()
{
	// We initialize SDL and create a window with it.
	SDL_Init(SDL_INIT_VIDEO);

	SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN);

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

	m_isInitialized = true;
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

		ConsoleLog::log(std::format("[{}: {}]", severity, type), color);
		ConsoleLog::log(pCallbackData->pMessage, ConsoleColor::White);
		std::cout << std::endl;
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
}

void VulkanEngine::InitSwapchain()
{
	vkb::SwapchainBuilder swapchainBuilder{ m_chosenGPU, m_device, m_surface };

	vkb::Swapchain vkbSwapchain = swapchainBuilder
	                                  .use_default_format_selection() // Use the default format (VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
	                                  .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR) // use vsync present mode
	                                  .set_desired_extent(m_windowExtent.width, m_windowExtent.height)
	                                  .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
	                                  .build()
	                                  .value();

	// store swapchain and its related images
	m_swapchain           = vkbSwapchain.swapchain;
	m_swapchainImages     = vkbSwapchain.get_images().value();
	m_swapchainImageViews = vkbSwapchain.get_image_views().value();

	m_swachainImageFormat = vkbSwapchain.image_format;
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
}

void VulkanEngine::Update()
{
	SDL_Event e;
	bool running = true;

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
		}

		Draw();
	}
}

void VulkanEngine::Draw()
{
	// wait until the gpu has finished rendering the last frame. Timeout of 1 second
	VK_CHECK(vkWaitForFences(m_device, 1, &GetCurrentFrame().m_renderFence, true, ONE_SECOND));
	VK_CHECK(vkResetFences(m_device, 1, &GetCurrentFrame().m_renderFence));

	// request image from the swapchain
	uint32_t swapchainImageIndex;
	VK_CHECK(vkAcquireNextImageKHR(m_device, m_swapchain, ONE_SECOND, GetCurrentFrame().m_swapchainSemaphore, nullptr, &swapchainImageIndex));

	// naming it cmd for shorter writing
	VkCommandBuffer cmd = GetCurrentFrame().m_mainCommandBuffer;

	// now that we are sure that the commands finished executing, we can safely
	// reset the command buffer to begin recording again.
	VK_CHECK(vkResetCommandBuffer(cmd, 0));

	// begin the command buffer recording. We will use this command buffer exactly once, so we want to let vulkan know that
	VkCommandBufferBeginInfo cmdBeginInfo = vkinit::CommandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	// start the command buffer recording
	VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

	// make the swapchain image into writeable mode before rendering
	vkutil::TransitionImage(cmd, m_swapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

	// make a clear-color from frame number. This will flash with a 120 frame period.
	VkClearColorValue clearValue;
	float flash = abs(sin(m_frameNumber / 120.f));
	clearValue  = { { 0.0f, 0.0f, flash, 1.0f } };

	VkImageSubresourceRange clearRange = vkinit::ImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT);

	// clear image
	vkCmdClearColorImage(cmd, m_swapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_GENERAL, &clearValue, 1, &clearRange);

	// make the swapchain image into presentable mode
	vkutil::TransitionImage(cmd, m_swapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

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
	// m_renderFence will now block until the graphic commands finish execution
	VK_CHECK(vkQueueSubmit2(m_graphicsQueue, 1, &submit, GetCurrentFrame().m_renderFence));

	// prepare present
	// this will put the image we just rendered to into the visible window.
	// we want to wait on the _renderSemaphore for that,
	// as its necessary that drawing commands have finished before the image is displayed to the user
	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType            = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext            = nullptr;
	presentInfo.pSwapchains      = &m_swapchain;
	presentInfo.swapchainCount   = 1;

	presentInfo.pWaitSemaphores    = &GetCurrentFrame().m_renderSemaphore;
	presentInfo.waitSemaphoreCount = 1;

	presentInfo.pImageIndices = &swapchainImageIndex;

	VK_CHECK(vkQueuePresentKHR(m_graphicsQueue, &presentInfo));

	// increase the number of frames drawn
	m_frameNumber++;
}

void VulkanEngine::Cleanup()
{
	if (m_isInitialized)
	{
		// make sure the gpu has stopped doing its things
		vkDeviceWaitIdle(m_device);

		for (int i = 0; i < FRAME_OVERLAP; i++)
		{

			// already written from before
			vkDestroyCommandPool(m_device, m_frames[i].m_commandPool, nullptr);

			// destroy sync objects
			vkDestroyFence(m_device, m_frames[i].m_renderFence, nullptr);
			vkDestroySemaphore(m_device, m_frames[i].m_renderSemaphore, nullptr);
			vkDestroySemaphore(m_device, m_frames[i].m_swapchainSemaphore, nullptr);
		}

		vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);

		// destroy swapchain resources
		for (int i = 0; i < m_swapchainImageViews.size(); i++)
		{
			vkDestroyImageView(m_device, m_swapchainImageViews[i], nullptr);
		}

		vkDestroyDevice(m_device, nullptr);
		vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
		vkb::destroy_debug_utils_messenger(m_instance, m_debug_messenger);
		vkDestroyInstance(m_instance, nullptr);
		SDL_DestroyWindow(m_window);
	}
}
