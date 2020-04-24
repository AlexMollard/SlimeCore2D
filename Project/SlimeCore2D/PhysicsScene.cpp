#include "PhysicsScene.h"
#include <iostream>
#include <tuple>
#include <sstream>
#include <unordered_map>
#include "Renderer2D.h"

PhysicsScene::PhysicsScene() : timeStep(0.01f), gravity(glm::vec2(0, 0))
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
	if (isKinematic)
		actor->SetMass(999999999999999.99f);

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
			

			// using function pointers
			int functionIdx = (int(object->GetType()) * int(ObjectType::ShapeCount)) + int(other->GetType());
			Collision_Function collisionFunctionPtr = collisionFunctions[functionIdx];

			if (collisionFunctionPtr != nullptr)
			{
				auto result = collisionFunctionPtr(object, other);
				if (glm::length(result) > 0.01)
				{
					if (other->GetType() != ObjectType::Line)
						other->SetNormal(result);

					other->ApplyOffSetToActor(object, glm::vec3(result,0));
					other->resolveCollision(object);
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
		Renderer2D::DrawQuad(glm::vec3(actors[i]->GetPos().x, actors[i]->GetPos().y, -0.8f), actors[i]->GetScale(), glm::vec4(1, 0, 0, 1));
	}

	Renderer2D::EndBatch();
	Renderer2D::Flush;
}

Collision_Function PhysicsScene::collisionFunctions[] =
{
	CollisionManager::CircleVsCircle, CollisionManager::CircleVsQuad, CollisionManager::CircleVsLine,
	CollisionManager::QuadVsCircle, CollisionManager::QuadVsQuad, CollisionManager::QuadVsLine,
	CollisionManager::LineVsCircle, CollisionManager::LineVsQuad, CollisionManager::LineVsLine
};