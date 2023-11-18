#include "CloudManager.h"
#include "engine/BatchRenderer.h"
#include <format>
#include <engine/ResourceManager.h>

CloudManager::CloudManager()
{
	for (int i = 0; i < CLOUD_TEXTURE_TOTAL; i++)
	{
		m_cloudTextures[i] = std::make_shared<Texture>(ResourceManager::GetTexturePath(std::format("Clouds/cloud_{}", i)));
		m_shadowTextures[i] = std::make_shared<Texture>(ResourceManager::GetTexturePath(std::format("Clouds/cloud_{}", i), "_shadow.png"));
	}
}

void CloudManager::Init(BatchRenderer* batchRenderer, int cloudTotal)
{
	for (int i = 0; i < cloudTotal; i++)
	{
		int textIndex = rand() % CLOUD_TEXTURE_TOTAL;

		auto cloud = std::make_shared<Cloud>(m_cloudTextures[textIndex].get(), m_shadowTextures[textIndex].get(), ((rand() % 20) * 0.1f) + 2, glm::vec2((rand() % 150 - 50), rand() % 100 - 50));

		m_clouds.push_back(cloud);
		batchRenderer->AddObject(cloud->GetCloud().get());
		batchRenderer->AddObject(cloud->GetShadow().get());
	}
}

void CloudManager::Update(float deltaTime)
{
	for (int i = 0; i < m_clouds.size(); i++)
	{
		m_clouds[i]->Update(deltaTime);
	}
}