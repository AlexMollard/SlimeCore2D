#pragma once
#include "glm.hpp"
#include "glew.h"
#include "glfw3.h"

class InputManager
{
	static InputManager* instance;

	InputManager();
	~InputManager();

	GLFWwindow* window;

	double mouseXPos, mouseYPos;
	int winWidth, winHeight;

	//16 by 9
	int aspectX, aspectY;

public:

	static InputManager* GetInstance() {
		if (!instance)
			instance = new InputManager;
		return instance;
	};

	void Update();

	glm::vec2 GetMousePos();
	glm::vec2 GetWindowSize();
	bool GetMouseDown(int button);
};

