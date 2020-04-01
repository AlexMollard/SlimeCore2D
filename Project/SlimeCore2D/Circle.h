#pragma once
#include "GameObject.h"
class Circle : public GameObject
{
public:
	Circle(Mesh* mesh);
	~Circle();

	void Update(float deltaTime);
};

