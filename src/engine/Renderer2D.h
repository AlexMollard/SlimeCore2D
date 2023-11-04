#pragma once
#include "Camera.h"
#include "GameObject.h"
#include "Shader.h"
#include "Texture.h"
#include <glm.hpp>
#include <memory>
#include <string>
#include <vector>
#include <array>

class Renderer2D
{
public:
	Renderer2D(Camera* camera);
	~Renderer2D();

	void Init();
	void ShutDown();

	void AddObject(GameObject* newObject);

	Texture* LoadTexture(const std::string& dir);

	void Draw();
	void DrawUI();

	Shader* GetBasicShader();

	void DrawUIQuad(glm::vec2 pos = glm::vec2(0), int layer = 1, glm::vec2 size = glm::vec2(1), glm::vec3 color = glm::vec3(1),
	                       Texture* texture = nullptr);

	void SetActiveRegion(Texture* texture, int regionIndex, int spriteWidth);

	void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
	void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad(glm::vec3(position, -0.9f), size, color);
	};
	void DrawQuad(glm::vec3 position, glm::vec2 size, glm::vec4 color, Texture* texture, int frame = 0, int spriteWidth = 16);

	void RemoveQuad(const GameObject& object);
	int GetObjectIndex(const GameObject& object);

	void BeginBatch();
	void EndBatch();
	void Flush();

private:
	glm::vec3 m_currentColor = glm::vec3(-404);
	glm::mat4 m_uiMatrix     = glm::ortho<float>(16, -16, 9, -9, 2, 4);

	Shader* m_currentShader   = nullptr;
	Texture* m_currentTexture = nullptr;

	std::vector<GameObject*> m_objectPool = std::vector<GameObject*>();
	std::vector<Texture*> m_texturePool = std::vector<Texture*>();
	std::vector<Shader*> m_shaderPool = std::vector<Shader*>();

	Shader m_basicShader = Shader("Basic Shader", "..\\Shaders\\BasicVertex.shader", "..\\Shaders\\BasicFragment.shader");
	Shader m_uiShader = Shader("UI Shader", "..\\Shaders\\UIVertex.shader", "..\\Shaders\\UIFragment.shader");
	Camera* m_camera = nullptr;

	std::vector<glm::vec2> m_uvs;
};
