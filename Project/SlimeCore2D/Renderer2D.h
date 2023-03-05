#pragma once
#include "Camera.h"
#include "Quad.h"
#include "Shader.h"
#include "Texture.h"
#include <glm.hpp>
#include <memory>
#include <string>
#include <vector>

class Renderer2D
{
public:
	Renderer2D(std::shared_ptr<Camera> camera);
	~Renderer2D();

	static void Init();
	static void ShutDown();

	void AddObject(std::shared_ptr<GameObject> newObject);

	std::shared_ptr<Texture> LoadTexture(const std::string& dir);

	void Draw();
	void DrawUI();

	std::shared_ptr<Shader> GetBasicShader();

	static void DrawUIQuad(glm::vec2 pos = glm::vec2(0), int layer = 1, glm::vec2 size = glm::vec2(1), glm::vec3 color = glm::vec3(1),
	                       std::shared_ptr<Texture> texture = nullptr);

	static void SetActiveRegion(std::shared_ptr<Texture> texture, int regionIndex, int spriteWidth);

	static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
	static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad(glm::vec3(position, -0.9f), size, color);
	};
	static void DrawQuad(glm::vec3 position, glm::vec2 size, glm::vec4 color, std::shared_ptr<Texture> texture, int frame = 0, int spriteWidth = 16);

	void RemoveQuad(std::shared_ptr<GameObject> object);
	int GetObjectIndex(std::shared_ptr<GameObject> object);

	static void BeginBatch();
	static void EndBatch();
	static void Flush();

private:
	glm::vec3 currentColor = glm::vec3(-404);
	glm::mat4 uiMatrix     = glm::ortho<float>(16, -16, 9, -9, 2, 4);

	std::shared_ptr<Shader> currentShader   = nullptr;
	std::shared_ptr<Texture> currentTexture = nullptr;

	std::vector<std::shared_ptr<GameObject>> objectPool;
	std::vector<std::shared_ptr<Texture>> texturePool;
	std::vector<std::shared_ptr<Shader>> shaderPool;

	static std::shared_ptr<Shader> basicShader;
	static std::shared_ptr<Shader> uiShader;
	static std::shared_ptr<Camera> camera;

	static std::vector<glm::vec2> uvs;
};
