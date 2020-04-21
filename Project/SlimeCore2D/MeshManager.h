#pragma once
#include "Mesh.h"
#include "GameObject.h"

class MeshManager
{
public:
	MeshManager();
	~MeshManager();

	void Draw(GameObject* object);

	void GetMesh(int index);
	void GetMesh(std::string name);
private:
	std::vector<Mesh*> meshes;
	Mesh* currentMesh = nullptr;

	Mesh* quad = nullptr;
	Mesh* circle = nullptr;
};

