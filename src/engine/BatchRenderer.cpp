#include "BatchRenderer.h"
#include <iostream>
#include "engine/GameObject.h"

BatchRenderer::~BatchRenderer()
{
	for (auto texture : m_texturePool)
	{
		delete texture;
		texture = nullptr;
	}
	m_texturePool.clear();

	// Objects are deleted in ObjectManager
	GetObjectPool().clear();
}

 BatchRenderer::BatchRenderer() 
 {
 }

void BatchRenderer::AddObject(GameObject* newObject)
{
	std::vector<GameObject*>& pool = GetObjectPool();
	auto it = std::find(pool.begin(), pool.end(), newObject);

	if (it != pool.end())
	{
		std::cout << "GameObject already in Renderer: " << &it << '\n';
		return;
	}

	newObject->SetID(pool.size());

	if (pool.empty() || pool.back()->GetPos().z <= newObject->GetPos().z)
	{
		pool.push_back(newObject);
	}
	else
	{
		for (auto i = pool.begin(); i != pool.end(); ++i)
		{
			if ((*i)->GetPos().z >= newObject->GetPos().z)
			{
				pool.insert(i, newObject);
				return;
			}
		}
	}
}

Texture* BatchRenderer::LoadTexture(const std::string& dir)
{
	auto texture = new Texture(dir);
	m_texturePool.emplace_back(texture);
	return texture;
}

void BatchRenderer::RemoveObject(const GameObject& object)
{
	std::vector<GameObject*>& pool = GetObjectPool();
	int index = 0;
	for (auto it = pool.begin(); it != pool.end(); ++it)
	{
		if (*it == &object)
		{
			// Shuffle everything down/over
			for (int i = pool.size() - 1; i >= index; i--)
			{
				pool.at(i + 1) = pool.at(i);
			}
			break;
		}
		index++;
	}
}

int BatchRenderer::GetObjectIndex(const GameObject& object)
{
	std::vector<GameObject*>& pool = GetObjectPool();
	for (int i = 0; i < pool.size(); i++)
	{
		if (pool[i] == &object)
		{
			return i;
		}
	}

	return -404;
}

RendererData* BatchRenderer::GetData() 
{
	return m_data;
}

RendererData* BatchRenderer::SetData(RendererData* data)
{
	m_data = data;
	return m_data;
}

std::vector<GameObject*>& BatchRenderer::GetObjectPool() 
{
	return m_objectPool;
}
