#include "CloudManager.h"

CloudManager::CloudManager(Renderer2D* renderer)
{
	this->renderer = renderer;
	
	for (int i = 0; i < CLOUD_TEXTURE_TOTAL; i++)
	{
		cloudTextures[i] = new Texture("..\\Textures\\Clouds\\cloud_" + std::to_string(i) + ".png");
		shadowTextures[i] = new Texture("..\\Textures\\Clouds\\cloud_" + std::to_string(i) + "_shadow.png");
	}


}

CloudManager::~CloudManager()
{
	for (int i = 0; i < CLOUD_TEXTURE_TOTAL; i++)
	{
		delete cloudTextures[i];
		cloudTextures[i] = nullptr;

		delete shadowTextures[i];
		shadowTextures[i] = nullptr;
	}

	for (int i = 0; i < clouds.size(); i++)
	{
		delete clouds[i];
		clouds[i] = nullptr;
	}
}

void CloudManager::Init(int cloudTotal)
{
	for (int i = 0; i < cloudTotal; i++)
	{
		int textIndex = rand() % CLOUD_TEXTURE_TOTAL;

		clouds.push_back(new Cloud(cloudTextures[textIndex], shadowTextures[textIndex], ((rand() % 20) * 0.1f) + 2,glm::vec2((rand() % 150 - 50), rand() % 100 - 50)));
		renderer->AddObject(clouds.back()->GetCloud());
		renderer->AddObject(clouds.back()->GetShadow());
	}
}

void CloudManager::Update(float deltaTime)
{
	for (int i = 0; i < clouds.size(); i++)
	{
		clouds[i]->Update(deltaTime);
	}
}
