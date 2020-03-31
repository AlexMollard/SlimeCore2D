#include "Window.h"
#include "Game2D.h"

InputManager* InputManager::instance = 0;

int main()
{	
	Window* app = new Window(1280, 720, (char*)"SlimeCore2D");
	Game2D* game = new Game2D();
	InputManager* inputManager = InputManager::GetInstance();

	while (!app->Window_shouldClose())
	{
		app->Update_Window();
		inputManager->Update();

		game->Update(app->GetDeltaTime());
		game->Draw();
	}

	delete app;
	app = nullptr;

	delete game;
	game = nullptr;

	delete InputManager::GetInstance();

	return 0;
}