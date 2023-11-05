#pragma once
#include "Cloud.h"

#define CLOUD_TEXTURE_TOTAL 3
#include <memory>

class BatchRenderer;
class CloudManager
{
public:
	CloudManager();
	~CloudManager() = default;

	void Init(BatchRenderer* batchRenderer, int cloudTotal);

	void Update(float deltaTime);

private:
	std::shared_ptr<Texture> m_cloudTextures[CLOUD_TEXTURE_TOTAL];
	std::shared_ptr<Texture> m_shadowTextures[CLOUD_TEXTURE_TOTAL];

	std::vector<std::shared_ptr<Cloud>> m_clouds;
};
