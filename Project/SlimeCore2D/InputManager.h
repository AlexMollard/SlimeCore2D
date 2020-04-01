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
	glm::vec2 GetAspectRatio();
	bool GetMouseDown(int button);

private:
	static InputManager* instance;

	InputManager();

	GLFWwindow* window;

	double mouseXPos = 0;
	double mouseYPos = 0;

	int winWidth = 0;
	int winHeight = 0;

	double aspectX = 32;
	double aspectY = 18;

	glm::vec2 deltaMouse = glm::vec2();
};
