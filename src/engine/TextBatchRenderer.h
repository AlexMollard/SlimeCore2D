#pragma once
#include "BatchRenderer.h"
class TextBatchRenderer : public BatchRenderer
{
public:
	void AddObject(GameObject* newObject) override;

	uint32_t GetTextureIndex(Texture* texture) override;
	uint32_t AddTextureSlot(Texture* texture)  override;
	uint32_t AddTextureSlot(GLuint textureID)  override;

	void RemoveObject(const GameObject& object) override;
	int GetObjectIndex(const GameObject& object) override;

	void ShutDown() override;

	void BeginBatch() override;
	void EndBatch() override;
	void Flush() override;

	void Render(const glm::vec2& camPos, float distanceFromCenter) override;
};
