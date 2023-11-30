#pragma once
#include "DescriptorLayoutBuilder.h"
#include "VulkanInit.h"
#include <deque>
#include <functional>
#include "ext/vector_float4.hpp"

constexpr unsigned int FRAME_OVERLAP = 2;

struct DeletionQueue
{
	std::deque<std::function<void()>> deletors;

	void push_function(std::function<void()>&& function)
	{
		deletors.push_back(function);
	}

	void flush()
	{
		// reverse iterate the deletion queue to execute all the functions
		for (auto it = deletors.rbegin(); it != deletors.rend(); it++)
		{
			(*it)(); // call functors
		}

		deletors.clear();
	}
};

struct ComputePushConstants
{
	glm::vec4 data1;
	glm::vec4 data2;
	glm::vec4 data3;
	glm::vec4 data4;
};

struct ComputeEffect
{
	const char* name;

	VkPipeline pipeline;
	VkPipelineLayout layout;

	ComputePushConstants data;
};

struct FrameData
{
	VkCommandPool m_commandPool;
	VkCommandBuffer m_mainCommandBuffer;
	VkSemaphore m_swapchainSemaphore;
	VkSemaphore m_renderSemaphore;
	VkFence m_renderFence;
	DeletionQueue m_deletionQueue;
};

class VulkanEngine
{
public:
	bool m_isInitialized = false;
	int m_frameNumber    = 0;

	VkExtent2D m_windowExtent{ 1700, 900 };
	struct SDL_Window* m_window = nullptr;

	void Init();
	void RebuildSwapchain();
	void Update();
	void Draw();
	void Render(VkCommandBuffer cmd);
	void DrawImgui(VkCommandBuffer cmd, VkImageView targetImageView);
	void Cleanup();

	void ImmediateSubmit(std::function<void(VkCommandBuffer cmd)>&& function);

	VkInstance m_instance;                      // Vulkan library handle
	VkDebugUtilsMessengerEXT m_debug_messenger; // Vulkan debug output handle
	VkPhysicalDevice m_chosenGPU;               // GPU chosen as the default device
	VkDevice m_device;                          // Vulkan device for commands
	VkSurfaceKHR m_surface;                     // Vulkan window surface

	VkSwapchainKHR m_swapchain;     // Swapchain to present images to the screen
	VkFormat m_swapchainImageFormat; // Format of the images in the swapchain

	std::vector<VkImage> m_swapchainImages;         // Images that belong to the swapchain
	std::vector<VkImageView> m_swapchainImageViews; // Image views for the swapchain images

	FrameData m_frames[FRAME_OVERLAP];

	FrameData& GetCurrentFrame()
	{
		return m_frames[m_frameNumber % FRAME_OVERLAP];
	};

	VkQueue m_graphicsQueue;
	uint32_t m_graphicsQueueFamily;
	VmaAllocator m_allocator;

	AllocatedImage m_drawImage;

	DescriptorAllocator globalDescriptorAllocator;

	VkDescriptorSet m_mainDescriptorSet;
	VkDescriptorSetLayout m_mainDescriptorLayout;

	VkPipelineLayout m_gradientPipelineLayout;

	// immediate submit structures (imm == immediate)
	VkFence m_immFence;
	VkCommandBuffer m_immCommandBuffer;
	VkCommandPool m_immCommandPool;

private:
	void InitVulkan();
	void InitSwapchain();
	void InitCommands();
	void InitSyncStructures();
	void InitDescriptors();
	void InitPipelines();
	void InitImgui();

	DeletionQueue m_mainDeletionQueue;

	std::vector<ComputeEffect> backgroundEffects;
	int currentBackgroundEffect = 0;
};
