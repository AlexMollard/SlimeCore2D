#include "PhysicsScene.h"
#include <iostream>
#include <sstream>

PhysicsScene::PhysicsScene() : timeStep(0.01f), gravity(glm::vec2(0, -3.2f))
{
	tex = new TextRenderer();
}

void PhysicsScene::addActor(RigidBody* actor, std::string name, bool isKinematic)
{
	for (int i = 0; i < actors.size(); i++)
	{
		if (actors[i] == actor)
		{
			std::cout << "Actor already exist in physics scene" << std::endl;
			break;
		}
	}
	actor->name = name;
	actor->SetKinematic(isKinematic);
	actors.push_back(actor);
}

void PhysicsScene::addActor(std::vector<RigidBody*> actors)
{
	addActor(actors.data(), actors.size());
}

void PhysicsScene::addActor(RigidBody** actors, int amount)
{
	for (int i = 0; i < amount; i++)
		this->actors.push_back(actors[i]);
}

void PhysicsScene::removeActor(RigidBody* actor)
{
	auto location = std::find(actors.begin(), actors.end(), actor);
	actors.erase(location);
}

void PhysicsScene::update(float dt) {

	// update physics at a fixed time step
	static float accumulatedTime = 0.0f;
	accumulatedTime += dt;
	while (accumulatedTime >= timeStep)
	{
		for (auto pActor : actors)
		{
			pActor->fixedUpdate(gravity, timeStep);
		}
		accumulatedTime -= timeStep;
	}

	static std::vector<RigidBody*> dirty;
	
	for (auto object : actors)
	{
		for (auto other : actors)
		{
			if (object == other)
				continue;
	
			if (std::find(dirty.begin(), dirty.end(), object) != dirty.end() && std::find(dirty.begin(), dirty.end(), other) != dirty.end())
			{
				continue;
			}
	
	
			if (object->GetIsColliding(other))
			{
				glm::vec2 overLap = GetOverLap(*other->GetBoundingBox(), *object->GetBoundingBox());
	
				other->ApplyOffSetToActor(object, overLap);
				other->resolveCollision(object);

				//std::cout << object->name << " Collided with: " << other->name << std::endl;
	
				dirty.push_back(object);
				dirty.push_back(other);
			}
		}
	}
		dirty.clear();
}

void PhysicsScene::Debug()
{
	for (int i = 0; i < actors.size(); i++)
	{
		std::ostringstream ss;
		ss << actors[i]->GetVelocity().x;
		std::string x(ss.str());

		ss << actors[i]->GetVelocity().y;
		std::string y(ss.str());

		std::string text = std::string(actors[i]->name + " Velocity: (" + x + ", " + y + ")");
		tex->RenderText(text, 50, 20 * i + 50, 1, glm::vec3(1));
	}
}

glm::vec2 PhysicsScene::GetOverLap(BoundingBox& one, BoundingBox& two)
{
	glm::vec2 overlap = { 0,0 };
	float overlap_size = 999999999999.0f;
	bool hasChanged = false;
	
	float a = abs(one.GetMax().x - two.GetMin().x);
	if (a < overlap_size)
	{
		overlap_size = a;
		overlap.x = -a;
		hasChanged = true;
	}
	
	float b = abs(two.GetMax().x - one.GetMin().x);
	if (b < overlap_size)
	{
		overlap = { 0,0 };
		overlap_size = b;
		overlap.x = b;
		hasChanged = true;
	}
	
	float c = abs(one.GetMax().y - two.GetMin().y);
	if (c < overlap_size)
	{
		overlap = { 0,0 };
		overlap_size = c;
		overlap.y = -c;
		hasChanged = true;
	}
	
	float d = abs(two.GetMax().y - one.GetMin().y);
	if (d < overlap_size)
	{
		overlap = { 0,0 };
		overlap_size = d;
		overlap.y = d;
		hasChanged = true;
	}

	return (hasChanged) ? overlap : glm::vec2(0);
}