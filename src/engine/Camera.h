#pragma once
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"

class Camera
{
public:
	Camera(float aspectX, float aspectY, float near, float far);
	~Camera();

	void Update(float deltaTime);
	void CameraMoveMent(float deltaTime);

	void SetPosition(glm::vec2 newPos);
	glm::vec2 GetPosition();

	glm::mat4 GetTransform();
	void UpdateTransform();

	void SetAspectRatio(glm::vec2 newAspectRatio);
	glm::vec2 GetAspectRatio();

	void SetClippingPlane(float newNear, float newFar);
	glm::vec2 GetClippingPlane();

	float GetFOV();
	void SetFOV(float newFOV);

private:
	glm::vec2 m_position = glm::vec2(0);
	glm::mat4 m_defaultTransform = glm::mat4(1);
	glm::mat4 m_transform = glm::mat4(1);
	glm::vec2 m_aspectRatio = glm::vec2(1);
	glm::vec2 m_aspectRatioBeforeFieldOfView = glm::vec2(1);

	float m_nearPlane = 0;
	float m_farPlane = 0;

	// 1.0f is default
	float m_fieldOfView = 1.0f;
};
