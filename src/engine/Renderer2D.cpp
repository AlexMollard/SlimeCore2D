#include "Renderer2D.h"
#include "engine/MemoryDebugging.h"

#include "Math.h"
#include <algorithm>
#include <array>
#include <iostream>
#include <memory>
#include <numeric>
#include "BatchRenderer.h"
#include <tuple>
#include "ResourceManager.h"
#include "Constants.h"

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

	ResourceManager* resourceManager = ResourceManager::GetInstance();
	
	Shader* basicShader = resourceManager->LoadShader("basicShader", ResourceManager::GetShaderPath("BasicVertex").c_str(), ResourceManager::GetShaderPath("BasicFragment").c_str());
	m_shaderMap.try_emplace(ShaderTypeToString(ShaderType::BASIC), basicShader);

	Shader* uiShader = resourceManager->LoadShader("uiShader", ResourceManager::GetShaderPath("UIVertex").c_str(), ResourceManager::GetShaderPath("UIFragment").c_str());
	m_shaderMap.try_emplace(ShaderTypeToString(ShaderType::UI), uiShader);
	
	Shader* gradientShader = resourceManager->LoadShader("gradientShader", ResourceManager::GetShaderPath("BasicVertex").c_str(), ResourceManager::GetShaderPath("GradientFragment").c_str());
	m_shaderMap.try_emplace(ShaderTypeToString(ShaderType::GRADIENT), gradientShader);

	Shader* waterShader = resourceManager->LoadShader("waterShader", ResourceManager::GetShaderPath("UIVertex").c_str(), ResourceManager::GetShaderPath("WaterFragment").c_str());
	m_shaderMap.try_emplace(ShaderTypeToString(ShaderType::WATER), waterShader);

	basicShader->AddTextureSlotsToShader<MAX_TEXTURE_COUNT>();
	uiShader->AddTextureSlotsToShader<MAX_TEXTURE_COUNT>();
	gradientShader->AddTextureSlotsToShader<1>();
	waterShader->AddTextureSlotsToShader<5>();
}

Renderer2D::~Renderer2D()
{
	m_camera = nullptr;
}
