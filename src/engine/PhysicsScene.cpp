
#include "PhysicsScene.h"

#include "Renderer2D.h"
#include <iostream>
#include <sstream>
#include <tuple>
#include <unordered_map>

void PhysicsScene::addActor(RigidBody* actor, std::string name, bool isKinematic)
{
	if (std::find(m_actors.begin(), m_actors.end(), actor) != m_actors.end())
	{
		std::cout << "Actor already exists in physics scene" << std::endl;
		return;
	}

	actor->name = name;
	actor->SetKinematic(isKinematic);

	if (isKinematic)
		actor->SetMass(std::numeric_limits<float>::max());
	else
		m_dynamicActors.push_back(actor);

	m_actors.push_back(actor);
}

void PhysicsScene::addActor(const std::vector<RigidBody*>& actors)
{
	for (auto actor : actors)
		addActor(actor);
}

void PhysicsScene::addActor(RigidBody** actors, int amount)
{
	for (int i = 0; i < amount; i++)
		addActor(actors[i]);
}

void PhysicsScene::removeActor(RigidBody* actor)
{
	m_actors.erase(std::remove(m_actors.begin(), m_actors.end(), actor), m_actors.end());
}

void PhysicsScene::update(float dt)
{
	// update physics at a fixed time step
	static float accumulatedTime = 0.0f;
	accumulatedTime += dt;
	while (accumulatedTime >= m_timeStep)
	{
		for (auto& actor : m_actors)
		{
			actor->fixedUpdate(m_gravity, m_timeStep);
		}
		accumulatedTime -= m_timeStep;
	}

	for (auto& dynamicActor : m_dynamicActors)
	{
		for (auto& surroundingTile : dynamicActor->GetSurroundingTiles())
		{
			if (surroundingTile && surroundingTile->GetKinematic())
			{
				const auto result = CollisionManager::QuadVsQuad(dynamicActor, surroundingTile);
				if (glm::length(result) > 0.01f)
				{
					surroundingTile->SetNormal(result);
					surroundingTile->ApplyOffSetToActor(dynamicActor, glm::vec3(result, 0.0f));
					surroundingTile->ResolveCollision(dynamicActor);
				}
			}
		}
	}
}

void PhysicsScene::setGravity(const glm::vec3 inGravity)
{
	m_gravity = inGravity;
}

glm::vec2 PhysicsScene::getGravity() const
{
	return m_gravity;
}

void PhysicsScene::setTimeStep(const float inTimeStep)
{
	m_timeStep = inTimeStep;
}

float PhysicsScene::getTimeStep() const
{
	return m_timeStep;
}
