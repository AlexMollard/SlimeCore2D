#include "Renderer2D.h"
#include <algorithm>

Renderer2D::Renderer2D(MeshManager* meshManager)
{
	this->meshManager = meshManager;

	basicShader = new Shader("Basic Shader", "..\\Shaders\\BasicVertex.shader", "..\\Shaders\\BasicFragment.shader");

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
	go->SetShader(basicShader);
	objectPool.push_back(go);
}

Texture* Renderer2D::LoadTexture(std::string dir)
{
	Texture* tempTex = new Texture(dir);

	texturePool.push_back(tempTex);
	return tempTex;
}

void Renderer2D::Draw()
{
	currentShader = nullptr;

	glActiveTexture(GL_TEXTURE0);
	texturePool[0]->Bind();
	currentTexture = texturePool[0];
	
	for (int i = 0; i < objectPool.size(); i++)
	{
		if (objectPool[i]->GetShader() != currentShader || currentShader == nullptr)
		{
			currentShader = objectPool[i]->GetShader();
			currentShader->Use();
			currentShader->setMat4("OrthoMatrix", orthoMatrix);
		}

		if (currentColor != objectPool[i]->GetColor())
		{
			currentColor = objectPool[i]->GetColor();
			currentShader->setVec3("color", currentColor);
		}

		currentShader->setMat4("Model", objectPool[i]->GetModel());
		
		meshManager->Draw(objectPool[i]);
	}
}

Shader* Renderer2D::GetBasicShader()
{
	return basicShader;
}