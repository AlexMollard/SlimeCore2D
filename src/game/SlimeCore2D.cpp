#include "engine/MemoryDebugging.h"
#include "engine/Window.h"
#include "Game2D.h"
#include <engine/ConsoleLog.h>


Input* Input::m_instance = 0;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// Create an instance of CustomStream and associate it with std::cout
	CustomStream customStream;
	std::cout.rdbuf(customStream.rdbuf());

	// Start time 
	float startTime = (float)SDL_GetTicks();

	Window app = Window(RES_WIDTH, RES_HEIGHT, "SlimeCore2D");
	Game2D game = Game2D();
	Input* inputManager = Input::GetInstance();
	inputManager->SetWindow(app.GetWindow());

	// Output how long it took to start the game
	SLIME_INFO("Game started in: {0}ms", (SDL_GetTicks() - startTime));

	// Game loop
	while (SDL_GetWindowFlags(app.GetWindow()) & SDL_WINDOW_SHOWN)
	{
		inputManager->Update();
		app.Update_Window();

		game.Update(app.GetDeltaTime());
		game.Draw();
	}

	delete Input::GetInstance();

	return 0;
}