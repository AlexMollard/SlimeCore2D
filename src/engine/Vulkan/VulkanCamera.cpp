#include "VulkanCamera.h"

#include "gtx/quaternion.hpp"
#include "gtx/transform.hpp"

using namespace vkutil;

glm::mat4 vkutil::Camera::CalculateProjectionMatrix(float aspectRatio)
{
	return glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
}

glm::mat4 Camera::GetViewMatrix()
{
	glm::mat4 cameraRotation = GetRotationMatrix();
	glm::mat4 cameraTranslation = glm::translate(glm::mat4(1.f), position);
	return glm::inverse(cameraTranslation * cameraRotation);
}

glm::mat4 Camera::GetRotationMatrix()
{
	glm::quat pitchRotation = glm::angleAxis(pitch, glm::vec3{ 1.f, 0.f, 0.f });
	glm::quat yawRotation   = glm::angleAxis(yaw, glm::vec3{ 0.f, -1.f, 0.f });

	return glm::toMat4(yawRotation) * glm::toMat4(pitchRotation);
}

void Camera::ProcessSDLEvent(SDL_Event& e)
{
	if (e.type == SDL_EVENT_KEY_DOWN)
	{
		if (e.key.keysym.sym == SDLK_w)
		{
			velocity.z = -1;
		}
		if (e.key.keysym.sym == SDLK_s)
		{
			velocity.z = 1;
		}
		if (e.key.keysym.sym == SDLK_a)
		{
			velocity.x = -1;
		}
		if (e.key.keysym.sym == SDLK_d)
		{
			velocity.x = 1;
		}
	}

	if (e.type == SDL_EVENT_KEY_UP)
	{
		if (e.key.keysym.sym == SDLK_w)
		{
			velocity.z = 0;
		}
		if (e.key.keysym.sym == SDLK_s)
		{
			velocity.z = 0;
		}
		if (e.key.keysym.sym == SDLK_a)
		{
			velocity.x = 0;
		}
		if (e.key.keysym.sym == SDLK_d)
		{
			velocity.x = 0;
		}
	}

	// If the mouse  is moving and the left button is down
	if (e.type == SDL_EVENT_MOUSE_MOTION && e.motion.state & SDL_BUTTON_LMASK)
	{
		yaw += (float)e.motion.xrel / 200.f;
		pitch -= (float)e.motion.yrel / 200.f;
	}
}

void Camera::Update()
{
	glm::mat4 cameraRotation = GetRotationMatrix();
	position += glm::vec3(cameraRotation * glm::vec4(velocity * 0.5f, 0.f));
}
