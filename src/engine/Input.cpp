#include "Input.h"
#include "engine/MemoryDebugging.h"

#include <iostream>
#include <string>

float Input::m_scroll;
int Input::m_mouseXPos;
int Input::m_mouseYPos;

// Most of this is broken because of the camera changing aspect ratio and position
void Input::Update()
{
	m_aspectX = -m_camera->GetAspectRatio().x;
	m_aspectY = -m_camera->GetAspectRatio().y;

	m_deltaMouse = glm::vec2((float)m_mouseXPos, (float)m_mouseYPos);

	// Get cursor pos
	SDL_GetMouseState(&m_mouseXPos, &m_mouseYPos);

	// Get window size
	SDL_GetWindowSize(m_window, &m_winWidth, &m_winHeight);

	m_mouseXPos /= (m_winWidth / (m_aspectX * 2));
	m_mouseXPos -= m_aspectX;

	m_mouseYPos /= (m_winHeight / (m_aspectY * 2));
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
	return (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(button));
}

void Input::SetCamera(Camera* cam)
{
	m_camera = cam;
}

SDL_Window* Input::GetWindow()
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

void Input::SetWindow(SDL_Window* window) 
{
	m_window = window;
}

void window_focus_callback(SDL_Window* window, int focused)
{
	Input::GetInstance()->SetFocus(focused);
}

void scroll_callback(SDL_Window* window, double xoffset, double yoffset)
{
	Input::SetScroll(yoffset);
}

bool Input::GetKeyPress(SDL_Keycode key)
{
	int state = SDL_GetKeyboardState(NULL)[SDL_GetScancodeFromKey(key)];
	if (state == SDL_PRESSED)
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