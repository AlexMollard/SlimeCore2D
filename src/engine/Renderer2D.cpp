#include "pch.h"
#include "Renderer2D.h"

#include "Math.h"
#include <algorithm>
#include <array>
#include <iostream>
#include <memory>
#include <numeric>
#include "BatchRenderer.h"
#include <tuple>

std::string ShaderTypeToString(ShaderType type)
{
	switch (type)
	{
		case ShaderType::BASIC:
			return "basic";
		case ShaderType::UI:
			return "UI";
		case ShaderType::GRADIENT:
			return "gradient";
		case ShaderType::WATER:
			return "water";
		default:
			return "basic";
	}
}

Renderer2D::Renderer2D(Camera* camera, Camera* screenCamera) : m_camera(camera), m_screenCamera(screenCamera)
{
	glClearColor(0.16f, 0.30f, 0.5f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	// Disabled for now, as it causes issues with the water shader (Flip policies in the batchrenderer)
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glfwSwapInterval(0); // V-Sync

	m_shaderMap.try_emplace(ShaderTypeToString(ShaderType::BASIC),    new Shader(ResourceManager::GetShaderPath("BasicVertex").c_str(), ResourceManager::GetShaderPath("BasicFragment").c_str()));
	m_shaderMap.try_emplace(ShaderTypeToString(ShaderType::UI),       new Shader(ResourceManager::GetShaderPath("UIVertex").c_str(),    ResourceManager::GetShaderPath("UIFragment").c_str()));
	m_shaderMap.try_emplace(ShaderTypeToString(ShaderType::GRADIENT), new Shader(ResourceManager::GetShaderPath("BasicVertex").c_str(), ResourceManager::GetShaderPath("GradientFragment").c_str()));
	m_shaderMap.try_emplace(ShaderTypeToString(ShaderType::WATER),    new Shader(ResourceManager::GetShaderPath("UIVertex").c_str(), ResourceManager::GetShaderPath("WaterFragment").c_str()));

	AddTextureSlotsToShader<MAX_TEXTURE_COUNT>(GetShader(ShaderType::BASIC));
	AddTextureSlotsToShader<MAX_TEXTURE_COUNT>(GetShader(ShaderType::UI));
	AddTextureSlotsToShader<1>(GetShader(ShaderType::GRADIENT));
	AddTextureSlotsToShader<5>(GetShader(ShaderType::WATER));
}

Renderer2D::~Renderer2D()
{
	for (auto& shader : m_shaderMap)
	{
		delete shader.second;
		shader.second = nullptr;
	}

	m_camera = nullptr;
}

template<int textureCount>
void Renderer2D::AddTextureSlotsToShader(Shader* shader)
{
	shader->Use();
	const auto loc = glGetUniformLocation(shader->GetID(), "Textures");
	std::array<int, textureCount> samplers{};
	std::iota(samplers.begin(), samplers.end(), 0);
	glUniform1iv(loc, textureCount, samplers.data());
}

Shader* Renderer2D::GetShader(const char* shaderName)
{
	return m_shaderMap[shaderName];
}

Shader* Renderer2D::GetShader(ShaderType shaderType)
{
	return m_shaderMap[ShaderTypeToString(shaderType)];
}
