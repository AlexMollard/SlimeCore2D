#include "Camera.h"
#include "InputManager.h"

Camera::Camera(float aspectX, float aspectY, float near, float far)
{
	transform = glm::ortho<float>(-aspectX, aspectX, -aspectY, aspectY, near, far);
	defaultTransform = transform;

	aspectRatio = glm::vec2(aspectX,aspectY);
	aspectRatioBeforeFieldOfView = aspectRatio;
	nearPlane = near;
	farPlane = far;
}

Camera::~Camera()
{
}

void Camera::Update()
{
	CameraMoveMent();
}

void Camera::CameraMoveMent()
{
	float moveSpeed = (InputManager::GetKeyPress(Keycode::LEFT_SHIFT)) ? 0.3f : 0.1f;

	if (InputManager::GetKeyPress(Keycode::W))
		position += glm::vec2(0.0f, -moveSpeed);

	if (InputManager::GetKeyPress(Keycode::S))
		position += glm::vec2(0.0f, moveSpeed);

	if (InputManager::GetKeyPress(Keycode::A))
		position += glm::vec2(moveSpeed, 0.0f);

	if (InputManager::GetKeyPress(Keycode::D))
		position += glm::vec2(-moveSpeed, 0.0f);

	if (InputManager::GetScroll() > 0)
		SetFOV(fieldOfView - moveSpeed * 0.25f);

	if (InputManager::GetScroll() < 0)
		SetFOV(fieldOfView + moveSpeed * 0.25f);

	UpdateTransform();
}

void Camera::SetPosition(glm::vec2 newPos)
{
	position = newPos;
	UpdateTransform();
}

glm::vec2 Camera::GetPosition()
{
	return position;
}

glm::mat4 Camera::GetTransform()
{
	return  transform;
}

void Camera::UpdateTransform()
{
	transform = defaultTransform;

	transform = glm::translate(transform, { position , 0.0f});
}

void Camera::SetAspectRatio(glm::vec2 newAspectRatio)
{
	aspectRatio = newAspectRatio;
	aspectRatioBeforeFieldOfView = aspectRatio;
	aspectRatio *= glm::vec2(fieldOfView);

	transform = glm::ortho<float>(-aspectRatio.x, aspectRatio.x, -aspectRatio.y, aspectRatio.y, nearPlane, farPlane);
	defaultTransform = transform;
}

glm::vec2 Camera::GetAspectRatio()
{
	return aspectRatio;
}

void Camera::SetClippingPlane(float newNear, float newFar)
{
	nearPlane = newNear;
	farPlane = newFar;

	transform = glm::ortho<float>(-aspectRatio.x, aspectRatio.x, -aspectRatio.y, aspectRatio.y, nearPlane, farPlane);
	defaultTransform = transform;
}

glm::vec2 Camera::GetClippingPlane()
{
	return glm::vec2(nearPlane, farPlane);
}

float Camera::GetFOV()
{
	return fieldOfView;
}

void Camera::SetFOV(float newFOV)
{
	fieldOfView = (newFOV >= 0.01f) ? newFOV : 0.01f;
	SetAspectRatio(aspectRatioBeforeFieldOfView);
}
