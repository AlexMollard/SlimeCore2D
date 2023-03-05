#include "PhysicsScene.h"

#include "Renderer2D.h"
#include <iostream>
#include <sstream>
#include <tuple>
#include <unordered_map>

PhysicsScene::PhysicsScene() : timeStep(0.01f), gravity(glm::vec2(0, 0)) {}

void PhysicsScene::addActor(RigidBody* actor, std::string name, bool isKinematic)
{
	if (std::find(actors.begin(), actors.end(), actor) != actors.end())
	{
		std::cout << "Actor already exists in physics scene" << std::endl;
		return;
	}

	actor->name = name;
	actor->SetKinematic(isKinematic);

	if (isKinematic)
		actor->SetMass(std::numeric_limits<float>::max());
	else
		dynamicActors.push_back(actor);

	actors.push_back(actor);
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
	actors.erase(std::remove(actors.begin(), actors.end(), actor), actors.end());
}

void PhysicsScene::update(float dt)
{
	// update physics at a fixed time step
	static float accumulatedTime = 0.0f;
	accumulatedTime += dt;
	while (accumulatedTime >= timeStep)
	{
		for (auto& actor : actors)
		{
			actor->fixedUpdate(gravity, timeStep);
		}
		accumulatedTime -= timeStep;
	}

	for (auto& dynamicActor : dynamicActors)
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

void PhysicsScene::Debug()
{
	Renderer2D::BeginBatch();

	for (int i = 0; i < actors.size(); i++)
	{
		glm::vec2 pos   = (actors[i]->GetUseBoundingBox()) ? actors[i]->GetBoundingBox().GetPos(actors[i]->GetPos()) : actors[i]->GetPos();
		glm::vec2 scale = (actors[i]->GetUseBoundingBox()) ? actors[i]->GetBoundingBox().GetScale() : actors[i]->GetScale();
		Renderer2D::DrawQuad(glm::vec3(pos.x, pos.y, -0.8f), scale, glm::vec4(1, 0, 0, 1));
	}

	Renderer2D::EndBatch();
	Renderer2D::Flush;
}