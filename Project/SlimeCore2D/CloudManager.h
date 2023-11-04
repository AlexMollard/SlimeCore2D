#pragma once
#include "Cloud.h"
#define CLOUD_TEXTURE_TOTAL 3
#include "Renderer2D.h"
#include <memory>
class CloudManager
{
public:
	CloudManager(Renderer2D* renderer);
	~CloudManager() = default;

	void Init(int cloudTotal);

	void Update(float deltaTime);

private:
	std::shared_ptr<Texture> m_cloudTextures[CLOUD_TEXTURE_TOTAL];
	std::shared_ptr<Texture> m_shadowTextures[CLOUD_TEXTURE_TOTAL];

	std::vector<std::shared_ptr<Cloud>> m_clouds;
	Renderer2D* m_renderer;
};
