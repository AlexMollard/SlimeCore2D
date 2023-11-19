#include <Windows.h>

#include "engine/MemoryDebugging.h"
#include "engine/Window.h"
#include "Game2D.h"


Input* Input::m_instance = 0;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Window app = Window(RES_WIDTH, RES_HEIGHT, "SlimeCore2D");
	Game2D game = Game2D();
	Input* inputManager = Input::GetInstance();
	inputManager->SetWindow(app.GetWindow());

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