#pragma once
#include "glm.hpp"
#include "glew.h"
#include "SDL3/SDL.h"
#include "Camera.h"

class Input
{
public:
	~Input() = default;

	static Input* GetInstance() {
		if (!m_instance)
			m_instance = new Input;
		return m_instance;
	};

	void Update();

	SDL_Window* GetWindow();

	static glm::vec2 GetMousePos();
	glm::vec2 GetDeltaMouse() const;

	glm::vec2 GetWindowSize() const;
	glm::vec2 GetAspectRatio() const;
	static bool GetMouseDown(int button);

	void SetCamera(Camera* cam);

	static bool GetKeyPress(SDL_Keycode key);

	static void SetScroll(float newScroll);
	static float GetScroll();

	bool GetFocus();
	void SetFocus(bool focus);

	static glm::vec2 GetMouseToWorldPos();

	void SetWindow(SDL_Window* window);
private:
	static Input* m_instance;

	Input() = default;

	SDL_Window* m_window;

	static float m_mouseXPos;
	static float m_mouseYPos;

	int m_winWidth = 0;
	int m_winHeight = 0;

	int m_aspectX = 32;
	int m_aspectY = 18;

	bool m_isWindowFocused = true;

	static float m_scroll;
	glm::vec2 m_deltaMouse = glm::vec2();

	Camera* m_camera = nullptr;
};
