#pragma once

#include "SDL3/SDL_events.h"
#include "VulkanTypes.h"

namespace vkutil
{
class Camera
{
public:
	glm::vec3 velocity;
	glm::vec3 position;
	// vertical rotation
	float pitch{ 0.f };
	// horizontal rotation
	float yaw{ 0.f };

	glm::mat4 GetViewMatrix();
	glm::mat4 GetRotationMatrix();

	void ProcessSDLEvent(SDL_Event& e);

	void Update();
};
} // namespace vkutil