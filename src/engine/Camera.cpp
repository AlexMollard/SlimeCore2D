#include "pch.h"
#include "Camera.h"
#include "Input.h"

Camera::Camera(float aspectX, float aspectY, float near, float far, bool moveCamera) : m_cameraMove(moveCamera)
{
	m_transform = glm::ortho<float>(-aspectX, aspectX, -aspectY, aspectY, near, far);
	m_defaultTransform = m_transform;

	m_aspectRatio = glm::vec2(aspectX, aspectY);
	m_aspectRatioBeforeFieldOfView = m_aspectRatio;
	m_nearPlane = near;
	m_farPlane = far;
}

Camera::~Camera()
{
}

void Camera::Update(float deltaTime)
{
	if (m_cameraMove)
	{
		CameraMoveMent(deltaTime);
	}
}

void Camera::CameraMoveMent(float deltaTime)
{
	float speed = (Input::GetKeyPress(Keycode::LEFT_SHIFT)) ? 10.0f : 4.0f;

	float moveSpeed = speed * deltaTime;

	if (Input::GetScroll() > 0)
		SetFOV(m_fieldOfView - moveSpeed);
	
	if (Input::GetScroll() < 0)
		SetFOV(m_fieldOfView + moveSpeed);

	UpdateTransform();
}

void Camera::SetPosition(glm::vec2 newPos)
{
	m_position = newPos;
	UpdateTransform();
}

glm::vec2 Camera::GetPosition()
{
	return m_position;
}

glm::mat4& Camera::GetTransform()
{
	return  m_transform;
}

void Camera::UpdateTransform()
{
	m_transform = m_defaultTransform;

	m_transform = glm::translate(m_transform, { m_position , 0.0f });
}

void Camera::SetAspectRatio(glm::vec2 newAspectRatio)
{
	m_aspectRatio = newAspectRatio;
	m_aspectRatioBeforeFieldOfView = m_aspectRatio;
	m_aspectRatio *= glm::vec2(m_fieldOfView);

	m_transform = glm::ortho<float>(-m_aspectRatio.x, m_aspectRatio.x, -m_aspectRatio.y, m_aspectRatio.y, m_nearPlane, m_farPlane);
	m_defaultTransform = m_transform;
}

glm::vec2 Camera::GetAspectRatio()
{
	return m_aspectRatio;
}

void Camera::SetClippingPlane(float newNear, float newFar)
{
	m_nearPlane = newNear;
	m_farPlane = newFar;

	m_transform = glm::ortho<float>(-m_aspectRatio.x, m_aspectRatio.x, -m_aspectRatio.y, m_aspectRatio.y, m_nearPlane, m_farPlane);
	m_defaultTransform = m_transform;
}

glm::vec2 Camera::GetClippingPlane()
{
	return glm::vec2(m_nearPlane, m_farPlane);
}

float Camera::GetFOV()
{
	return m_fieldOfView;
}

void Camera::SetFOV(float newFOV)
{
	m_fieldOfView = (newFOV >= 0.01f) ? newFOV : 0.01f;
	SetAspectRatio(m_aspectRatioBeforeFieldOfView);
}