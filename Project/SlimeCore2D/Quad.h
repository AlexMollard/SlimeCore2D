#pragma once
#include "GameObject.h"

class Quad : public GameObject
{
public:
	Quad(Mesh* mesh);
	~Quad();

	void Update(float deltaTime);
};
