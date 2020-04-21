#pragma once
#include "GameObject.h"
class Circle : public GameObject
{
public:
	Circle();
	~Circle();

	void Update(float deltaTime);
};

