#pragma once
#include "glm.hpp"
#include "InputManager.h"

class BoundingBox
{
public:
	BoundingBox();
	~BoundingBox();

	void UpdateBoundingBox(glm::vec3 pos, glm::vec3 size);

	bool GetIsColliding(glm::vec2 other);
	bool GetIsColliding(float x, float y);

	bool GetMouseColliding();

private:
	InputManager* inputManager = InputManager::GetInstance();

	glm::vec2 topRight = glm::vec2(0);
	glm::vec2 bottomLeft = glm::vec2(0);
};

