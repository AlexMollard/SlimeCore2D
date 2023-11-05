#include "pch.h"
#include "Renderer2D.h"

#include "Math.h"
#include <algorithm>
#include <array>
#include <iostream>
#include <memory>
#include <numeric>
#include "BatchRenderer.h"

Renderer2D::Renderer2D(Camera* camera) : m_camera(camera)
{
	switch (m_shaderType)
	{
	case ShaderType::Basic:
		m_shader = new Shader("Basic Shader", ResourceManager::GetShaderPath("BasicVertex").c_str(), ResourceManager::GetShaderPath("BasicFragment").c_str());
		break;
	case ShaderType::UI:
		m_shader = new Shader("UI Shader", ResourceManager::GetShaderPath("UIVertex").c_str(), ResourceManager::GetShaderPath("UIFragment").c_str());
		break;
	default:
		break;
	}

	m_shader->Use();

	const auto loc = glGetUniformLocation(m_shader->GetID(), "Textures");
	std::array<int, MAX_TEXTURE_COUNT> samplers{};
	std::iota(samplers.begin(), samplers.end(), 0);

	glUniform1iv(loc, MAX_TEXTURE_COUNT, samplers.data());
}

Renderer2D::~Renderer2D()
{
	delete m_shader;
	m_shader = nullptr;

	m_camera = nullptr;
}

void Renderer2D::Draw(BatchRenderer* batchRenderer)
{
	batchRenderer->BeginBatch();

	m_shader->Use();
	m_shader->setMat4("OrthoMatrix", m_camera->GetTransform());
	m_shader->setMat4("Model", glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)));
	m_shader->setVec4("SunColor", glm::vec4(1.0f));

	glm::vec2 camPos = m_camera->GetPosition();
	float distanceFromCenter = -m_camera->GetAspectRatio().x + 6;

	batchRenderer->Render(camPos, distanceFromCenter);

	batchRenderer->EndBatch();
	batchRenderer->Flush();
}

Shader* Renderer2D::GetShader()
{
	return m_shader;
}