#pragma once
#include "BatchRenderer.h"

struct QuadBatchData
{
	glm::vec3 position    = glm::vec3(0.0f);
	glm::vec2 anchorPoint = glm::vec2(0.5f);
	glm::vec2 size        = glm::vec2(1.0f);
	glm::vec4 color       = glm::vec4(1.0f);
	glm::vec4 uvRect          = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f); // left, top, width, height
	float rotation        = 0.0f;
	Texture* texture      = nullptr;
	Texture* maskTexture      = nullptr;
	FlipPolicy flipPolicy = FlipPolicy::None;
};

class QuadBatchRenderer : public BatchRenderer
{
public:
	QuadBatchRenderer();
	~QuadBatchRenderer() override;

	void Render(const glm::vec2& camPos, float distanceFromCenter) override;

	uint32_t GetTextureIndex(Texture* texture) override;
	uint32_t AddTextureSlot(Texture* texture) override;
	uint32_t AddTextureSlot(GLuint textureID) override;

	float GetZOffset() const;
	void SetZOffset(float val);

	bool GetOcclusionCulling() const;
	void SetOcclusionCulling(bool val);
private:
	void DrawQuad(const QuadBatchData& batchData);
	void SetSpriteUvs(glm::vec4 uvRect);

	void ShutDown() override;

	void BeginBatch() override;
	void EndBatch() override;
	void Flush() override;

	std::vector<glm::vec2> m_uvs;

	float m_zOffset = 0.0f;
	bool m_occulsionCulling = true;

	const glm::vec2 basicUVs[4] = { glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f) };
};
