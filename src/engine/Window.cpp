#include "Window.h"
#include "engine/MemoryDebugging.h"
#include "ConsoleLog.h"
#include <string>

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
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		SLIME_ERROR("Failed to initialize SDL");
		return -1;
	}


    // Output the SDL version in green
	SDL_version sdlVersion;
	SDL_GetVersion(&sdlVersion);
	ConsoleLog::log(std::format("SDL Version: {}.{}.{}", sdlVersion.major, sdlVersion.minor, sdlVersion.patch), ConsoleColor::Green);

	// Create window
	window = SDL_CreateWindow(name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	
	// Create OpenGL context
	glContext = SDL_GL_CreateContext(window);

    // Initialize GLEW after creating the OpenGL context
	GLenum glewInitResult = glewInit();
	if (glewInitResult != GLEW_OK)
	{
		SLIME_ERROR("Failed to initialize GLEW");
		return -1;
	}

	// Check for OpenGL support
	if (!GLEW_VERSION_3_3)
	{
		SLIME_ERROR("OpenGL 3.3 is not supported");
		return -1;
	}

	// Setup OpenGL viewport
	glViewport(0, 0, width, height);

	// Setup OpenGL options
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Setup OpenGL callbacks
	SDL_GL_SetSwapInterval(1); // Enable vsync

	// If anything has gone wrong, exit and return -1
	if (!window || !glContext)
	{
		SLIME_ERROR("Failed to create window or OpenGL context");
		return -1;
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
	std::cout << "  Max nr of texture units supported: " << nrTextureUnits << "\n" << std::endl;

	return 1;
}

void Window::Update_Window()
{
	SDL_GL_SwapWindow(window);
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			Window_destroy();
		}

		if (event.type == SDL_WINDOWEVENT)
		{
			if (event.window.event == SDL_WINDOWEVENT_RESIZED)
			{
				glViewport(0, 0, event.window.data1, event.window.data2);
			}
		}

		if (event.type == SDL_KEYDOWN)
		{
			if (event.key.keysym.sym == SDLK_ESCAPE)
			{
				Window_destroy();
			}
		}
	}

	now   = SDL_GetTicks();
	delta = (now - last) / 1000.0f;
	last  = now;
}

void Window::Window_destroy()
{
	SDL_GL_DeleteContext(glContext);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

SDL_Window* Window::GetWindow() const
{
	return window;
}

float Window::GetDeltaTime()
{
	return delta;
}