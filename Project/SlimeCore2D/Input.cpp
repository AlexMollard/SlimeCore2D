#include "pch.h"
#include "Input.h"
#include <iostream>
#include <string>

// GLFW Callbacks
void window_focus_callback(GLFWwindow* window, int focused);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

float Input::m_scroll;
double Input::m_mouseXPos;
double Input::m_mouseYPos;

Input::Input()
{
	m_window = glfwGetCurrentContext();
	glfwSetWindowFocusCallback(m_window, window_focus_callback);
	glfwSetScrollCallback(m_window, scroll_callback);
}

Input::~Input()
{
}

// Most of this is broken because of the camera changing aspect ratio and position
void Input::Update()
{
	m_aspectX = -m_camera->GetAspectRatio().x;
	m_aspectY = -m_camera->GetAspectRatio().y;

	m_deltaMouse = glm::vec2((float)m_mouseXPos, (float)m_mouseYPos);

	glfwGetCursorPos(m_window, &m_mouseXPos, &m_mouseYPos);
	glfwGetWindowSize(m_window, &m_winWidth, &m_winHeight);

	m_mouseXPos /= (m_winWidth / (m_aspectX * 2.0f));
	m_mouseXPos -= m_aspectX;

	m_mouseYPos /= (m_winHeight / (m_aspectY * 2.0f));
	m_mouseYPos -= m_aspectY;
	m_mouseYPos = -m_mouseYPos;

	m_deltaMouse -= glm::vec2((float)m_mouseXPos, (float)m_mouseYPos);
	Input::m_scroll = 0.0f;
}

glm::vec2 Input::GetMousePos()
{
	return { m_mouseXPos, m_mouseYPos };
}

glm::vec2 Input::GetDeltaMouse() const
{
	return m_deltaMouse;
}

glm::vec2 Input::GetWindowSize() const
{
	return { m_winWidth, m_winHeight };
}

glm::vec2 Input::GetAspectRatio() const
{
	return { m_aspectX, m_aspectY };
}

bool Input::GetMouseDown(int button)
{
	return (glfwGetMouseButton(GetInstance()->m_window, button));
}

void Input::SetCamera(Camera* cam)
{
	m_camera = cam;
}

GLFWwindow* Input::GetWindow()
{
	return m_window;
}

bool Input::GetFocus()
{
	return m_isWindowFocused;
}

void Input::SetFocus(bool focus)
{
	m_isWindowFocused = focus;
}

glm::vec2 Input::GetMouseToWorldPos()
{
	return GetInstance()->m_camera->GetPosition() + (glm::vec2(m_mouseXPos, m_mouseYPos));
}

void window_focus_callback(GLFWwindow* window, int focused)
{
	Input::GetInstance()->SetFocus(focused);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	Input::SetScroll(yoffset);
}

bool Input::GetKeyPress(Keycode key)
{
	int state = glfwGetKey(GetInstance()->m_window, (int)key);
	if (state == GLFW_PRESS)
	{
		return true;
	}

	return false;
}

void Input::SetScroll(float newScroll)
{
	m_scroll = newScroll;
}

float Input::GetScroll()
{
	return Input::m_scroll;
}