#include "InputManager.h"

InputManager::InputManager()
{
	window = glfwGetCurrentContext();
}

InputManager::~InputManager()
{
}

void InputManager::Update()
{
	glfwGetCursorPos(window, &mouseXPos, &mouseYPos);
	glfwGetWindowSize(window, &winWidth, &winHeight);

	mouseXPos /= (winWidth / 32);
	mouseXPos -= 16;

	mouseYPos /= (winHeight / 18);
	mouseYPos -= 9;
	mouseYPos = -mouseYPos;
}

glm::vec2 InputManager::GetMousePos()
{
	return glm::vec2(mouseXPos, mouseYPos);
}

glm::vec2 InputManager::GetWindowSize()
{
	return glm::vec2(winWidth, winHeight);
}

bool InputManager::GetMouseDown(int button)
{
	return (glfwGetMouseButton(window, button));
}
