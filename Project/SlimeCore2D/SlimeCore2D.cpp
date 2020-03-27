#include "Window.h"
#include "Game2D.h"
int main()
{	
	Window* app = new Window(1280, 720, (char*)"SlimeCore2D");
	Game2D* game = new Game2D();

	while (!app->Window_shouldClose())
	{
		app->Update_Window();
		
		game->Draw();
		game->Update(app->GetDeltaTime());
	}

	delete app;
	delete game;

	return 0;
}