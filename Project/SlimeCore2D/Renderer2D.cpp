#include "Renderer2D.h"
#include <algorithm>

Renderer2D::Renderer2D()
{
	basicShader = new Shader("Basic Shader", "..\\Shaders\\BasicVertex.shader", "..\\Shaders\\BasicFragment.shader");
	
	quad = new Mesh("Cube");
	quad->CreateQuad();

	circle = new Mesh("Circle");
	circle->CreateCircle();

	LoadTexture("..\\Textures\\White.png");
	LoadTexture("..\\Textures\\Test.png");

	textRenderer = new TextRenderer();
}

Renderer2D::~Renderer2D()
{
	for (int i = 0; i < texturePool.size(); i++)
	{
		delete texturePool[i];
		texturePool[i] = nullptr;
	}

	delete basicShader;
	basicShader = nullptr;

	delete quad;
	quad = nullptr;

	delete circle;
	circle = nullptr;

	delete textRenderer;
	textRenderer = nullptr;
}

void Renderer2D::AddObject(GameObject* newObject)
{
	std::vector<GameObject*>::iterator it = find(objectPool.begin(), objectPool.end(), newObject);

	if (it != objectPool.end())
	{
		std::cout << "GameObject already in Renderer: " << *it << '\n';
		return;
	}

	GameObject* go = newObject;
	go->SetID(objectPool.size());
	objectPool.push_back(go);
}

Texture* Renderer2D::LoadTexture(std::string dir)
{
	Texture* tempTex = new Texture(dir);

	texturePool.push_back(tempTex);
	return tempTex;
}

Shader* Renderer2D::GetShaderFromtype(ObjectType type)
{
	return basicShader;
}

void Renderer2D::Draw()
{
	currentShader = nullptr;

	glActiveTexture(GL_TEXTURE0);
	texturePool[0]->Bind();
	
	for (int i = 0; i < objectPool.size(); i++)
	{
		if (GetShaderFromtype(objectPool[i]->GetType()) != currentShader || currentShader == nullptr)
		{
			currentShader = GetShaderFromtype(objectPool[i]->GetType());
			currentShader->Use();
			currentShader->setMat4("OrthoMatrix", orthoMatrix);
		}


		currentShader->setVec3("color", objectPool[i]->GetColor());
		currentShader->setMat4("Model", objectPool[i]->GetModel());

		objectPool[i]->Draw();
	}
}

Mesh* Renderer2D::GetQuadMesh()
{
	return quad;
}

Mesh* Renderer2D::GetCircleMesh()
{
	return circle;
}

Shader* Renderer2D::GetBasicShader()
{
	return basicShader;
}