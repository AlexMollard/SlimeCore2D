#pragma once
#include "glm.hpp"
#include "InputManager.h"

class BoundingBox
{
public:
	BoundingBox();
	~BoundingBox();

	void UpdateBoundingBox(glm::vec2 pos, glm::vec2 size);

	bool GetIsColliding(BoundingBox& other);

	bool GetMouseColliding();
	
	glm::vec2& GetMax();
	glm::vec2& GetMin();

private:
	InputManager* inputManager = InputManager::GetInstance();

	glm::vec2 max = glm::vec2(0);
	glm::vec2 min = glm::vec2(0);
};

