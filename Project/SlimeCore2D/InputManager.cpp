#include "InputManager.h"
#include <iostream>
InputManager::InputManager()
{
	window = glfwGetCurrentContext();
}

InputManager::~InputManager()
{
}

void InputManager::Update()
{
	deltaMouse = glm::vec2((float)mouseXPos, (float)mouseYPos);

	glfwGetCursorPos(window, &mouseXPos, &mouseYPos);
	glfwGetWindowSize(window, &winWidth, &winHeight);

	mouseXPos /= (winWidth / (aspectX * 2.0f));
	mouseXPos -= aspectX;

	mouseYPos /= (winHeight / (aspectY * 2.0f));
	mouseYPos -= aspectY;
	mouseYPos = -mouseYPos;

	deltaMouse -= glm::vec2((float)mouseXPos, (float)mouseYPos);
}

glm::vec2 InputManager::GetMousePos()
{
	return glm::vec2(mouseXPos, mouseYPos);
}

glm::vec2 InputManager::GetDeltaMouse()
{
	return deltaMouse;
}

glm::vec2 InputManager::GetWindowSize()
{
	return glm::vec2(winWidth, winHeight);
}

glm::vec2 InputManager::GetAspectRatio()
{
	return glm::vec2(aspectX,aspectY);
}

bool InputManager::GetMouseDown(int button)
{
	return (glfwGetMouseButton(window, button));
}
