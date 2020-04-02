#include "InputManager.h"
#include "Debug.h"
#include <iostream>
#include <string>

void window_focus_callback(GLFWwindow* window, int focused);

InputManager::InputManager()
{
	window = glfwGetCurrentContext();
	glfwSetWindowFocusCallback(window, window_focus_callback);
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
	return glm::vec2(aspectX, aspectY);
}

bool InputManager::GetMouseDown(int button)
{
	return (glfwGetMouseButton(window, button));
}

GLFWwindow* InputManager::GetWindow()
{
	return window;
}

bool InputManager::GetFocus()
{
	return IsWindowFocused;
}

void InputManager::SetFocus(bool focus)
{
	IsWindowFocused = focus;
}

void window_focus_callback(GLFWwindow* window, int focused)
{
		InputManager::GetInstance()->SetFocus(focused);
}