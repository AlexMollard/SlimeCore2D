#include "PhysicsScene.h"
#include <iostream>
#include <sstream>

PhysicsScene::PhysicsScene() : timeStep(0.01f), gravity(glm::vec2(0, -10.0f))
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

	for (int i = 0; i < actors.size(); i++)
	{
		RigidBody* object = actors[i];

		for (int y = i + 1; y < actors.size(); y++)
		{
			RigidBody* other = actors[y];

			if (other->GetKinematic() && object->GetKinematic())
				continue;

			if (std::find(object->collided.begin(), object->collided.end(), other) != object->collided.end())
				continue;

			// using function pointers
			int functionIdx = (int(object->GetType()) * int(ObjectType::ShapeCount)) + int(other->GetType());
			Collision_Function collisionFunctionPtr = collisionFunctions[functionIdx];

			if (collisionFunctionPtr != nullptr)
			{
				auto result = collisionFunctionPtr(object, other);
				if (glm::length(result) > 0.00001)
				{
					other->SetNormal(result);

					other->ApplyOffSetToActor(object, result);
					other->resolveCollision(object);

					other->collided.push_back(object);
				}
			}
		}
	}
	for (int i = 0; i < actors.size(); i++)
	{
		actors[i]->collided.clear();
	}
}

void PhysicsScene::Debug()
{
	std::cout.precision(2);

	for (int i = 0; i < actors.size(); i++)
	{
		if (actors[i]->GetKinematic())
			continue;

		std::ostringstream ss;
		ss << actors[i]->GetVelocity().x;
		std::string x(ss.str());

		ss << actors[i]->GetVelocity().y;
		std::string y(ss.str());

		std::string text = std::string(actors[i]->name + " Velocity: (" + x + ", " + y + ")");
		tex->RenderText(text, 50, 20 * i + 50, 1, glm::vec3(1));
	}
}

const Collision_Function PhysicsScene::collisionFunctions[] =
{
	CollisionManager::CircleVsCircle, CollisionManager::CircleVsQuad, CollisionManager::CircleVsLine,
	CollisionManager::QuadVsCircle, CollisionManager::QuadVsQuad, CollisionManager::QuadVsLine,
	CollisionManager::LineVsCircle, CollisionManager::LineVsQuad, CollisionManager::LineVsLine
};