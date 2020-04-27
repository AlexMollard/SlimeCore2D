#pragma once
#include "Cloud.h"
#define CLOUD_TEXTURE_TOTAL 3
#include "Renderer2D.h"
class CloudManager
{
public:
	CloudManager(Renderer2D* renderer);
	~CloudManager();

	void Init(int cloudTotal);

	void Update(float deltaTime);

private:
	Texture* cloudTextures[CLOUD_TEXTURE_TOTAL];
	Texture* shadowTextures[CLOUD_TEXTURE_TOTAL];

	std::vector<Cloud*> clouds;
	Renderer2D* renderer;
};
