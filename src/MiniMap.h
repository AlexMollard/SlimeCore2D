#pragma once
#include "engine/GameObject.h"

class MiniMap : public GameObject
{
public:
	MiniMap() = default;
	~MiniMap() = default;

	void Create(glm::vec3 pos, glm::vec3 color, glm::vec2 scale, int id) override;
	void Update(float deltaTime) override;
};

