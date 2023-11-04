#pragma once
#include "glm.hpp"
#include "Input.h"

class BoundingBox
{
public:
	BoundingBox() = default;
	~BoundingBox() = default;

	void UpdateQuadBoundingBox(glm::vec2 pos, glm::vec2 size);

	bool GetIsColliding(const BoundingBox& other) const;

	bool GetMouseColliding();

	glm::vec2& GetMax();
	glm::vec2& GetMin();

	glm::vec2 GetPos(glm::vec3 objectPos) const;
	glm::vec2 GetScale() const;

	glm::vec2 m_offset = glm::vec2(0);
	glm::vec2 m_scale = glm::vec2(0);
private:

	Input* m_inputManager = Input::GetInstance();

	glm::vec2 m_max = glm::vec2(0);
	glm::vec2 m_min = glm::vec2(0);
};
