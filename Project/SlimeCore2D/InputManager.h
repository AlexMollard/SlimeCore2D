#pragma once
#include "glm.hpp"
#include "glew.h"
#include "glfw3.h"

class InputManager
{
public:
	~InputManager();

	static InputManager* GetInstance() {
		if (!instance)
			instance = new InputManager;
		return instance;
	};

	void Update();

	glm::vec2 GetMousePos();
	glm::vec2 GetDeltaMouse();
	glm::vec2 GetWindowSize();
	bool GetMouseDown(int button);

private:
	static InputManager* instance;

	InputManager();

	GLFWwindow* window;

	double mouseXPos, mouseYPos;
	int winWidth, winHeight;

	double aspectX = 16;
	double aspectY = 9;

	glm::vec2 deltaMouse = glm::vec2();

};

