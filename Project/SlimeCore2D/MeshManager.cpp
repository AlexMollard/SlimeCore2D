#include "MeshManager.h"

MeshManager::MeshManager()
{
	quad = new Mesh("Cube");
	quad->CreateQuad();

	circle = new Mesh("Circle");
	circle->CreateCircle();
}

MeshManager::~MeshManager()
{
	delete quad;
	quad = nullptr;

	delete circle;
	circle = nullptr;
}

void MeshManager::Draw(GameObject* object)
{
	if (object->GetType() == ObjectType::Quad || object->GetType() == ObjectType::Line)
	{
		quad->draw();
		currentMesh = quad;
		return;
	}

	if (object->GetType() == ObjectType::Circle)
	{

		circle->draw();
		currentMesh = circle;
		return;
	}
}

void MeshManager::GetMesh(int index)
{
}

void MeshManager::GetMesh(std::string name)
{

}
