#pragma once
#include <vector>
#include "common.hpp"
#include "fwd.hpp"
#include "glew.h"
#include <array>

class GameObject;
class Texture;
struct RendererData;

class BatchRenderer
{
public:
	BatchRenderer();
	~BatchRenderer();

	void AddObject(GameObject* newObject);

	Texture* LoadTexture(const std::string& dir);

	void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
	void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);;
	void DrawQuad(glm::vec3 position, glm::vec2 size, glm::vec4 color, Texture* texture, int frame = 0, int spriteWidth = 16);

	void RemoveQuad(const GameObject& object);
	int GetObjectIndex(const GameObject& object);
	void SetActiveRegion(Texture* texture, int regionIndex, int spriteWidth);

	void ShutDown();

	void BeginBatch();
	void EndBatch();
	void Flush();

	void Render(const glm::vec2& camPos, float distanceFromCenter);
private:
	std::vector<GameObject*> m_objectPool = std::vector<GameObject*>();
	std::vector<Texture*> m_texturePool = std::vector<Texture*>();
	std::vector<glm::vec2> m_uvs;

	RendererData* m_data;

	const glm::vec2 basicUVs[4] = { glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 1.0f) };

};

