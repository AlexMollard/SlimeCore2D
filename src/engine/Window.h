#include <iostream>
#include <glew.h>
#include <SDL.h>

class Window
{
public:
	Window(int width, int height, const char* name);
	~Window();

	// Window Functions
	int Window_intit(int width, int height, const char* name);
	void Update_Window();
	void Window_destroy();

	SDL_Window* GetWindow() const;

	int GetWidth() const;
	int GetHeight() const;

	float GetDeltaTime();
protected:
	// Main Window
	SDL_Window* window;
	SDL_GLContext glContext;

	// Window size
	int m_width;
	int m_height;

	// DeltaTime
	double last = 0.0;
	double now = 0.0;
	float delta = 1.0f;
};
