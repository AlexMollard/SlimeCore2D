#pragma once
#include "glm.hpp"
#include "glew.h"
#include "glfw3.h"
#include "Keycode.h"
#include "Camera.h"

class Input
{
public:
	~Input();

	static Input* GetInstance() {
		if (!m_instance)
			m_instance = new Input;
		return m_instance;
	};

	void Update();

	GLFWwindow* GetWindow();

	static glm::vec2 GetMousePos();
	glm::vec2 GetDeltaMouse() const;

	glm::vec2 GetWindowSize() const;
	glm::vec2 GetAspectRatio() const;
	static bool GetMouseDown(int button);

	void SetCamera(Camera* cam);

	static bool GetKeyPress(Keycode key);

	static void SetScroll(float newScroll);
	static float GetScroll();

	bool GetFocus();
	void SetFocus(bool focus);

	static glm::vec2 GetMouseToWorldPos();
private:
	static Input* m_instance;

	Input();

	GLFWwindow* m_window;

	static double m_mouseXPos;
	static double m_mouseYPos;

	int m_winWidth = 0;
	int m_winHeight = 0;

	double m_aspectX = 32;
	double m_aspectY = 18;

	bool m_isWindowFocused = true;

	static float m_scroll;
	glm::vec2 m_deltaMouse = glm::vec2();

	Camera* m_camera = nullptr;
};
