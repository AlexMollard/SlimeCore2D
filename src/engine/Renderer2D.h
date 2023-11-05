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

class BatchRenderer;

enum class ShaderType
{
	Basic,
	UI
};

class Renderer2D
{
public:
	Renderer2D(Camera* camera);
	~Renderer2D();

	void Draw(BatchRenderer* batchRenderer);

	Shader* GetShader();
private:
	glm::vec3 m_currentColor = glm::vec3(-404);
	glm::mat4 m_uiMatrix     = glm::ortho<float>(16, -16, 9, -9, 2, 4);

	Shader* m_currentShader   = nullptr;
	Texture* m_currentTexture = nullptr;

	ShaderType m_shaderType = ShaderType::Basic;

	Shader* m_shader = nullptr;
	Camera* m_camera = nullptr;
};
