#pragma once
#include "GameObject.h"
class Line : public GameObject
{
public:
	Line(Mesh* mesh);
	~Line();

	void Update(float deltaTime);

	float GetDistance();
	void SetDistance(float newDistance);
protected:
	float distance = 0.0f;
};

