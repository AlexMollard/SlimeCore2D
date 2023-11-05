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
		default:
			return "basic";
	}
}

Renderer2D::Renderer2D(Camera* camera) : m_camera(camera)
{
	m_shaderMap.try_emplace(ShaderTypeToString(ShaderType::BASIC), new Shader(ResourceManager::GetShaderPath("BasicVertex").c_str(), ResourceManager::GetShaderPath("BasicFragment").c_str()));
	m_shaderMap.try_emplace(ShaderTypeToString(ShaderType::UI), new Shader(ResourceManager::GetShaderPath("UIVertex").c_str(), ResourceManager::GetShaderPath("UIFragment").c_str()));
	m_shaderMap.try_emplace(ShaderTypeToString(ShaderType::GRADIENT), new Shader(ResourceManager::GetShaderPath("BasicVertex").c_str(), ResourceManager::GetShaderPath("GradientFragment").c_str()));

	AddTextureSlotsToShader(GetShader(ShaderType::BASIC));
	AddTextureSlotsToShader(GetShader(ShaderType::UI));
	AddTextureSlotsToShader(GetShader(ShaderType::GRADIENT));
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

void Renderer2D::AddTextureSlotsToShader(Shader* shader)
{
	shader->Use();
	const auto loc = glGetUniformLocation(shader->GetID(), "Textures");
	std::array<int, MAX_TEXTURE_COUNT> samplers{};
	std::iota(samplers.begin(), samplers.end(), 0);
	glUniform1iv(loc, MAX_TEXTURE_COUNT, samplers.data());
}

Shader* Renderer2D::GetShader(const char* shaderName)
{
	return m_shaderMap[shaderName];
}

Shader* Renderer2D::GetShader(ShaderType shaderType)
{
	return m_shaderMap[ShaderTypeToString(shaderType)];
}
