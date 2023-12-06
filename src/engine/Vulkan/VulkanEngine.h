#pragma once

#include "volk.h"

#include "VulkanCamera.h"
#include "VulkanDescriptors.h"
#include "VulkanInit.h"
#include "VulkanLoader.h"
#include "engine/DeletionQueue.h"
#include "glm.hpp"
#include <queue>
#include <functional>
#include <memory>
#include <unordered_map>

constexpr unsigned int FRAME_OVERLAP = 2;

struct FrameData
{
	VkSemaphore m_swapchainSemaphore;
	VkSemaphore m_renderSemaphore;
	VkFence m_renderFence;

	vkutil::DescriptorAllocator m_frameDescriptors;
	DeletionQueue m_deletionQueue;

	VkCommandPool m_commandPool;
	VkCommandBuffer m_mainCommandBuffer;

	AllocatedBuffer cameraBuffer;
};

struct EngineStats
{
	float frameTime;
	int triangleCount;
	int drawcallCount;
	float meshDrawTime;

	std::deque<float> frameTimes;
	float fps;
};

class VulkanEngine
{
public:
	// Singleton pattern
	static VulkanEngine& Get()
	{
		static VulkanEngine instance;
		return instance;
	}

	// Initialization and cleanup
	void Init();
	void Cleanup();

	// Main loop functions
	void Update();
	void Draw();
	void DrawMain(VkCommandBuffer cmd);
	void DrawImgui(VkCommandBuffer cmd, VkImageView targetImageView);
	void DrawGeometry(VkCommandBuffer cmd);

	// Immediate submission
	void ImmediateSubmit(std::function<void(VkCommandBuffer cmd)>&& function);

	// Vulkan-related member variables
	VkInstance m_instance{};
	VkDebugUtilsMessengerEXT m_debug_messenger{};
	VkPhysicalDevice m_chosenGPU{};
	VkDevice m_device{};
	VkSurfaceKHR m_surface{};

	// Window-related member variables
	VkExtent2D m_windowExtent{ 1700, 900 };
	struct SDL_Window* m_window = nullptr;

	// Swapchain
	VkSwapchainKHR m_swapchain{};
	VkFormat m_swapchainImageFormat;
	std::vector<VkImage> m_swapchainImages;
	std::vector<VkImageView> m_swapchainImageViews;

	// Frame data
	int m_frameNumber = 0;
	FrameData m_frames[FRAME_OVERLAP];
	FrameData& GetCurrentFrame();

	// Queues and allocator
	VkQueue m_graphicsQueue{};
	uint32_t m_graphicsQueueFamily{};
	VmaAllocator m_allocator{};

	// Image and buffer resources
	VkFormat m_drawFormat;
	AllocatedImage m_drawImage{};
	AllocatedImage m_depthImage{};
	AllocatedImage m_whiteImage{};
	VkSampler m_defaultSampler{};

	// Descriptor sets and layouts
	vkutil::DescriptorAllocator globalDescriptorAllocator{};
	VkDescriptorSet m_mainDescriptorSet{};
	VkDescriptorSetLayout m_mainDescriptorLayout{};
	// Bindless descriptors
	vkutil::BindlessDescriptorWriter m_bindlessWriter;


	// Pipelines and pipeline layouts
	VkPipelineLayout m_gradientPipelineLayout{};
	VkPipelineLayout m_meshPipelineLayout{};
	VkPipelineLayout m_bindlessPipelineLayout{};
	VkPipeline m_meshPipeline{};
	VkPipeline m_bindlessPipeline{};

	// Default GLTF descriptor
	VkDescriptorSet m_defaultGLTFdescriptor{};

	// Draw image descriptors and layout
	VkDescriptorSet m_drawImageDescriptors{};
	VkDescriptorSetLayout m_drawImageDescriptorLayout{};

	// Other descriptor layouts
	VkDescriptorSetLayout m_gpuSceneDataDescriptorLayout{};
	VkDescriptorSetLayout m_gltfMatDescriptorLayout{};

	// Default GLTF materials
	MaterialData m_gltfDefaultOpaque{};
	MaterialData m_gltfDefaultTranslucent{};
	AllocatedBuffer m_defaultGLTFMaterialData{};

	// Immediate submit structures
	VkFence m_immFence{};
	VkCommandBuffer m_immCommandBuffer{};
	VkCommandPool m_immCommandPool{};
	DrawContext m_drawCommands;

	// Scene data, camera, and stats
	GPUSceneData m_sceneData{};
	vkutil::Camera mainCamera;
	EngineStats m_stats;

	// Mesh-related functions and data
	GPUMeshBuffers UploadMesh(std::span<uint32_t> indices, std::span<Vertex> vertices, const char* name);

	// Buffer creation and destruction
	AllocatedBuffer CreateBuffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage, const char* name = nullptr);
	void DestroyBuffer(const AllocatedBuffer& buffer);

	// Default data initialization
	void InitDefaultData();

	// Compute effect creation
	ComputeEffect* CreateComputeEffect(const char* name, VkShaderModule shader, VkPipelineLayout layout, VkComputePipelineCreateInfo computePipelineCreateInfo);

	// Image creation and destruction
	AllocatedImage CreateImage(void* data, VkExtent3D imageSize, VkFormat format, VkImageUsageFlags usage, const char* name = nullptr);
	AllocatedImage CreateImage(VkExtent3D size, VkFormat format, VkImageUsageFlags usage, const char* name = nullptr);
	void DestroyImage(AllocatedImage image);

	// Background effects
	std::vector<ComputeEffect*> backgroundEffects;
	int currentBackgroundEffect = 0;

	// Loaded scenes and deletion queue
	std::unordered_map<std::string, std::shared_ptr<LoadedGLTF>> m_loadedScenes;
	void AddToDeletionQueue(AllocatedImage image);
	void AddToDeletionQueue(VkImage image);
	void AddToDeletionQueue(VkImageView imageView);
	void AddToDeletionQueue(VkBuffer buffer);

	// Lastly function pointer deletion queue
	void AddToDeletionQueue(std::function<void()>&& function);

private:
	// Vulkan initialization and cleanup functions
	bool m_isInitialized = false;
	void InitVulkan();
	void CleanupSwapchain();

	// Swapchain and rendering setup functions
	void InitSwapchain();
	void InitCommands();
	void InitSyncStructures();
	void InitPipelines();
	void InitImgui();

	// Update functions
	bool HandleSDLEvents(SDL_Event& e);
	void UpdateSceneData();
	void UpdateImgui();

	bool m_skipDrawing = false;

	// Deletion queue for cleanup
	DeletionQueue m_mainDeletionQueue;
};
