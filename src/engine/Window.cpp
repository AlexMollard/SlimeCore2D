#include "Window.h"

#include "engine/MemoryDebugging.h"
#include "ConsoleLog.h"

#include "SDL3/SDL_vulkan.h"
#include "Vulkan/VulkanInstance.h"
#include "Vulkan/VulkanDevice.h"
#include "Vulkan/VulkanSwapchain.h"

#include <iostream>
#include <optional>
#include <set>
#include <string>
#include <vector>

Window::Window(int width, int height, const char* name)
{
	// Check for Memory Leaks
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	if (!WindowInit(width, height, name))
	{
		std::cout << "Failed to load window" << std::endl;
	}
}

Window::~Window()
{
}

int Window::WindowInit(int width, int height, const char* name)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		SLIME_ERROR("Failed to initialize SDL");
		return -1;
	}

	// Create the vulkan instance
	m_instance = new VulkanInstance(name, name);

	// Create window
	m_window = SDL_CreateWindow(name, width, height, SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN);

	// Create an SDL Vulkan SDL surface
	VkSurfaceKHR surface = static_cast<VkSurfaceKHR>(m_surface);
	if (!SDL_Vulkan_CreateSurface(m_window, *m_instance->GetInstance(), nullptr, &surface))
	{
		SLIME_ERROR("Failed to create Vulkan surface: {}", SDL_GetError());
	}
	m_surface = surface;
	
	// Create the vulkan device
	m_device = new VulkanDevice(*m_instance->GetInstance(), m_surface);

	// Create the vulkan swapchain
	m_swapchain = new VulkanSwapchain(*m_device, m_surface);

	return 1;
}

void Window::UpdateWindow()
{
	SDL_GL_SwapWindow(m_window);
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_EVENT_KEY_DOWN)
		{
			if (event.key.keysym.sym == SDLK_ESCAPE)
			{
				m_isRunning = false;
			}
		}
	}

	m_now   = SDL_GetTicks();
	m_delta = (m_now - m_last) / 1000.0f;
	m_last  = m_now;
}

void Window::WindowDestroy()
{
	delete m_swapchain;
	delete m_device;
	delete m_instance;

	SDL_DestroyWindow(m_window);
	SDL_Quit();
}

VkSurfaceKHR Window::GetSurface() const
{
	return m_surface;
}

VulkanInstance* Window::GetInstance()
{
	return m_instance;
}

VulkanDevice* Window::GetDevice()
{
	return m_device;
}

VkQueue Window::GetGraphicsQueue() const
{
	return m_graphicsQueue;
}

SDL_Window* Window::GetWindow()
{
	return m_window;
}

int Window::GetWidth() const
{
	return m_width;
}

int Window::GetHeight() const
{
	return m_height;
}

float Window::GetDeltaTime()
{
	return m_delta;
}

bool Window::GetRunning() const
{
	return m_isRunning;
}

void Window::SetRunning(bool val)
{
	m_isRunning = val;
}