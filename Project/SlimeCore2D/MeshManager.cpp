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
		if (currentMesh == quad)
		{
			glDrawElements(GL_TRIANGLES, quad->GetIndicesCount(), GL_UNSIGNED_INT, 0);
			return;
		}

		quad->draw();
		currentMesh = quad;
		return;
	}

	if (object->GetType() == ObjectType::Circle)
	{
		if (currentMesh == circle)
		{
			glDrawElements(GL_TRIANGLES, circle->GetIndicesCount(), GL_UNSIGNED_INT, 0);
			return;
		}

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
