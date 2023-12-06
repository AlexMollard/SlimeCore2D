#include <SDL.h>
#include "vulkan/volk.h"

class VulkanInstance;
class VulkanDevice;
class VulkanSwapchain;
class VulkanPipeline;

class Window
{
public:
	Window(int width, int height, const char* name);
	~Window();

	// Window Functions
	int WindowInit(int width, int height, const char* name);
	void UpdateWindow();
	void WindowDestroy();

	VkSurfaceKHR GetSurface() const;
	VulkanInstance* GetInstance();
	VulkanDevice* GetDevice();
	VkQueue GetGraphicsQueue() const;

	SDL_Window* GetWindow();

	int GetWidth() const;
	int GetHeight() const;

	float GetDeltaTime();

	bool GetRunning() const;
	void SetRunning(bool val);

protected:
	// Main Window
	SDL_Window* m_window;

	// Vulkan Stuff
	VulkanInstance* m_instance = nullptr;
	VulkanDevice* m_device = nullptr;
	VulkanSwapchain* m_swapchain = nullptr;
	VulkanPipeline* m_graphicsPipeline = nullptr;
	VkSurfaceKHR m_surface;
	VkPhysicalDevice m_physicalDevice;
	VkQueue m_graphicsQueue;

	// Window size
	int m_width;
	int m_height;

	// DeltaTime
	double m_last = 0.0;
	double m_now  = 0.0;
	float m_delta = 1.0f;

	bool m_isRunning = true;
};