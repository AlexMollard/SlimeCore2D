#include "engine/MemoryDebugging.h"
#include "engine/Vulkan/VulkanDebug.h"
#include "engine/Window.h"
#include "Game2D.h"
#include <engine/ConsoleLog.h>
#include <engine/ResourceManager.h>


Input* Input::m_instance = 0;
ResourceManager* ResourceManager::m_instance = 0;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// Create an instance of CustomStream and associate it with std::cout
	CustomStream customStream;
	std::cout.rdbuf(customStream.rdbuf());

	// Start time 
	float startTime = (float)SDL_GetTicks();

	Window app = Window(RES_WIDTH, RES_HEIGHT, "SlimeCore2D");
	Game2D game = Game2D();
	
	ResourceManager* resourceManager = ResourceManager::GetInstance();
	
	Input* inputManager = Input::GetInstance();
	inputManager->SetWindow(app.GetWindow());

	// Output how long it took to start the game
	SLIME_INFO("Game started in: {0}ms", (SDL_GetTicks() - startTime));

	// Game loop
	while (app.GetRunning())
	{
		inputManager->Update();
		app.UpdateWindow();

		game.Update(app.GetDeltaTime());
		game.Draw();
	}

	app.WindowDestroy();

	// Delete the resource manager
	delete ResourceManager::GetInstance();

	// Delete the input manager
	delete Input::GetInstance();

	return 0;
}