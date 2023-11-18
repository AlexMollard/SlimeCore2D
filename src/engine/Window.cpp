#include "Window.h"
#include "engine/MemoryDebugging.h"
#include "ConsoleLog.h"
#include <string>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

Window::Window(int width, int height, const char* name)
{
	// Check for Memory Leaks
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	if (!Window_intit(width, height, name))
	{
		std::cout << "Failed to load window" << std::endl;
	}
}

Window::~Window()
{
	Window_destroy();
}

int Window::Window_intit(int width, int height, const char* name)
{
	if (!glfwInit())
	{
		return -1;
	}

    // Output the GLFW version in green
	int major, minor, revision;
	glfwGetVersion(&major, &minor, &revision);
	ConsoleLog::log(std::format("GLFW Version: {}.{}.{}", major, minor, revision), ConsoleColor::Green);

	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	window = glfwCreateWindow(width, height, name, NULL, NULL);
	glfwMakeContextCurrent(window);

	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Initializing Glew
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Glew is not having a good time" << std::endl;
	}

	// Output more verbose info
	ConsoleLog::log("\nOpenGL Info:", ConsoleColor::Yellow);
	std::cout << "  Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "  Vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "  Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "  Shading Language Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

	// Output more info about the GPU and its abilities
	ConsoleLog::log("\nGPU Info:", ConsoleColor::Red);

	int nrAttributes;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
	std::cout << "  Max nr of vertex attributes supported: " << nrAttributes << std::endl;

	int nrTextureUnits;
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &nrTextureUnits);
	std::cout << "  Max nr of texture units supported: " << nrTextureUnits << std::endl;


	return 1;
}

void Window::Update_Window()
{
	glfwSwapBuffers(window);
	glfwPollEvents();

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	now = glfwGetTime();
	delta = (float)(now - last);
	last = now;
}

int Window::Window_shouldClose()
{
	return glfwWindowShouldClose(window);
}

void Window::Window_destroy()
{
	glfwDestroyWindow(window);
	glfwTerminate();
}

float Window::GetDeltaTime()
{
	return delta;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}