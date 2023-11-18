#pragma once
#include "CommonEnums.h"
#include "common.hpp"
#include "fwd.hpp"
#include "glew.h"
#include <array>
#include <vector>
#include <string>

class GameObject;
class Texture;
struct RendererData;

class BatchRenderer
{
public:
	BatchRenderer();
	virtual ~BatchRenderer();

	virtual void AddObject(GameObject* newObject);

	virtual Texture* LoadTexture(const std::string& dir);

	virtual uint32_t GetTextureIndex(Texture* texture) = 0;
	virtual uint32_t AddTextureSlot(Texture* texture) = 0;
	virtual uint32_t AddTextureSlot(GLuint textureID) = 0;

	virtual void RemoveObject(const GameObject& object);
	virtual int GetObjectIndex(const GameObject& object);

	virtual void ShutDown() = 0;

	virtual void BeginBatch() = 0;
	virtual void EndBatch() = 0;
	virtual void Flush() = 0;

	virtual void Render(const glm::vec2& camPos, float distanceFromCenter) = 0;

	RendererData* GetData();
	RendererData* SetData(RendererData* data);

	std::vector<GameObject*>& GetObjectPool();

private:

	bool m_renderBufferValid = false;

	std::vector<GameObject*> m_objectPool = std::vector<GameObject*>();
	std::vector<Texture*> m_texturePool   = std::vector<Texture*>();

	RendererData* m_data = nullptr;
};
