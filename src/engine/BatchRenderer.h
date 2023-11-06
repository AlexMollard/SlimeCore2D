#pragma once
#include <vector>
#include "common.hpp"
#include "fwd.hpp"
#include "glew.h"
#include <array>
#include "CommonEnums.h"

class GameObject;
class Texture;
struct RendererData;

struct BatchData
{
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec2 size = glm::vec2(1.0f);
	glm::vec4 color = glm::vec4(1.0f);
	float rotation = 0.0f;
	Texture* texture = nullptr;
	FlipPolicy flipPolicy = FlipPolicy::None;

	int spriteFrame = 0;
	int spriteWidth = 0;
};

class BatchRenderer
{
public:
	BatchRenderer();
	~BatchRenderer();

	void AddObject(GameObject* newObject);

	Texture* LoadTexture(const std::string& dir);

	void DrawQuad(const BatchData& batchData);

	uint32_t GetTextureIndex(Texture* texture);
	uint32_t AddTextureSlot(Texture* texture);
	uint32_t AddTextureSlot(GLuint textureID);


	void RemoveQuad(const GameObject& object);
	int GetObjectIndex(const GameObject& object);
	void SetActiveRegion(Texture* texture, int regionIndex, int spriteWidth);

	void ShutDown();

	void BeginBatch();
	void EndBatch();
	void Flush();

	void Render(const glm::vec2& camPos, float distanceFromCenter);
private:
	bool m_renderBufferValid = false;

	std::vector<GameObject*> m_objectPool = std::vector<GameObject*>();
	std::vector<Texture*> m_texturePool = std::vector<Texture*>();
	std::vector<glm::vec2> m_uvs;

	RendererData* m_data;

	const glm::vec2 basicUVs[4] = { glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f) };

};

