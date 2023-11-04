#include "pch.h"
#include "CloudManager.h"

CloudManager::CloudManager(Renderer2D* renderer)
{
	this->m_renderer = renderer;

	for (int i = 0; i < CLOUD_TEXTURE_TOTAL; i++)
	{
		m_cloudTextures[i] = std::make_shared<Texture>("..\\Textures\\Clouds\\cloud_" + std::to_string(i) + ".png");
		m_shadowTextures[i] = std::make_shared<Texture>("..\\Textures\\Clouds\\cloud_" + std::to_string(i) + "_shadow.png");
	}
}

void CloudManager::Init(int cloudTotal)
{
	for (int i = 0; i < cloudTotal; i++)
	{
		int textIndex = rand() % CLOUD_TEXTURE_TOTAL;

	//	auto cloud = std::make_shared<Cloud>(cloudTextures[textIndex], shadowTextures[textIndex], ((rand() % 20) * 0.1f) + 2, glm::vec2((rand() % 150 - 50), rand() % 100 - 50));

		//clouds.push_back(cloud);
		//renderer->AddObject(cloud->GetCloud());
		//renderer->AddObject(cloud->GetShadow());
	}
}

void CloudManager::Update(float deltaTime)
{
	for (int i = 0; i < m_clouds.size(); i++)
	{
		m_clouds[i]->Update(deltaTime);
	}
}