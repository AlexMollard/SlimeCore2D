#pragma once
#include "VulkanCamera.h"
#include "VulkanDescriptors.h"
#include "VulkanInit.h"
#include "VulkanLoader.h"
#include "glm.hpp"
#include <deque>
#include <functional>
#include <memory>
#include <unordered_map>

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

struct RenderObject
{
	uint32_t indexCount;
	uint32_t firstIndex;
	VkBuffer indexBuffer;

	MaterialData* material;

	glm::mat4 transform;
	VkDeviceAddress vertexBufferAddress;
};

struct DrawContext
{
	std::vector<RenderObject> OpaqueSurfaces;
	std::vector<RenderObject> TransparentSurfaces;
};

struct FrameData
{
	VkSemaphore m_swapchainSemaphore;
	VkSemaphore m_renderSemaphore;
	VkFence m_renderFence;

	DescriptorAllocator m_frameDescriptors;
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
};

class VulkanEngine
{
public:
	static VulkanEngine& Get()
	{
		static VulkanEngine instance;
		return instance;
	}

	bool m_isInitialized = false;
	int m_frameNumber    = 0;

	VkExtent2D m_windowExtent{ 1700, 900 };
	struct SDL_Window* m_window = nullptr;

	void Init();
	void Update();
	void Draw();
	void DrawMain(VkCommandBuffer cmd);
	void DrawImgui(VkCommandBuffer cmd, VkImageView targetImageView);
	void DrawGeometry(VkCommandBuffer cmd);
	void Cleanup();

	void ImmediateSubmit(std::function<void(VkCommandBuffer cmd)>&& function);

	VkInstance m_instance;                      // Vulkan library handle
	VkDebugUtilsMessengerEXT m_debug_messenger; // Vulkan debug output handle
	VkPhysicalDevice m_chosenGPU;               // GPU chosen as the default device
	VkDevice m_device;                          // Vulkan device for commands
	VkSurfaceKHR m_surface;                     // Vulkan window surface

	VkSwapchainKHR m_swapchain;      // Swapchain to present images to the screen
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

	VkFormat m_drawFormat;
	AllocatedImage m_drawImage;
	AllocatedImage m_depthImage;

	AllocatedImage m_whiteImage;
	VkSampler m_defaultSampler;

	DescriptorAllocator globalDescriptorAllocator;

	VkDescriptorSet m_mainDescriptorSet;
	VkDescriptorSetLayout m_mainDescriptorLayout;

	VkPipelineLayout m_gradientPipelineLayout;

	VkPipelineLayout m_meshPipelineLayout;
	VkPipeline m_meshPipeline;

	VkDescriptorSet m_defaultGLTFdescriptor;

	VkDescriptorSet m_drawImageDescriptors;
	VkDescriptorSetLayout m_drawImageDescriptorLayout;

	VkDescriptorSetLayout m_gpuSceneDataDescriptorLayout;
	VkDescriptorSetLayout m_gltfMatDescriptorLayout;

	MaterialData m_gltfDefaultOpaque;
	MaterialData m_gltfDefaultTranslucent;
	AllocatedBuffer m_defaultGLTFMaterialData;

	// immediate submit structures (imm == immediate)
	VkFence m_immFence;
	VkCommandBuffer m_immCommandBuffer;
	VkCommandPool m_immCommandPool;
	DrawContext m_drawCommands;

	GPUSceneData m_sceneData;
	vkutil::Camera mainCamera;
	EngineStats m_stats;

	GPUMeshBuffers UploadMesh(std::span<uint32_t> indices, std::span<Vertex> vertices);
	std::vector<MeshAsset> testMeshes;

	AllocatedBuffer CreateBuffer(size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage);
	void DestroyBuffer(const AllocatedBuffer& buffer);
	void InitDefaultData();

	ComputeEffect* CreateComputeEffect(const char* name, VkShaderModule shader, VkPipelineLayout layout, VkComputePipelineCreateInfo computePipelineCreateInfo);

	AllocatedImage CreateImage(void* data, VkExtent3D imageSize, VkFormat format, VkImageUsageFlags usage);
	AllocatedImage CreateImage(VkExtent3D size, VkFormat format, VkImageUsageFlags usage);
	void DestroyImage(AllocatedImage image);

	std::unordered_map<std::string, std::shared_ptr<LoadedGLTF>> m_loadedScenes;

private:
	void InitVulkan();
	void InitSwapchain();
	void InitCommands();
	void InitSyncStructures();
	void InitDescriptors();
	void InitPipelines();
	void InitBackgroundPipelines();
	void InitMeshPipeline();
	void InitImgui();

	DeletionQueue m_mainDeletionQueue;

	std::vector<ComputeEffect*> backgroundEffects;
	int currentBackgroundEffect = 0;
};
