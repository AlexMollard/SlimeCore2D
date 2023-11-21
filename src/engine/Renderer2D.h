#pragma once
#include "BatchRenderer.h"
#include "Camera.h"
#include "GameObject.h"
#include "Shader.h"
#include "Texture.h"
#include <array>
#include <glm.hpp>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

class BatchRenderer;

enum class CameraType
{
	ORTHOGRAPHIC, // default
	SCREENSPACE,  // UI and other screen space elements
	PERSPECTIVE,  // 3D not implemented yet
	COUNT
};

// If you add a shader here, make sure to add it to ShaderTypeToString() in Renderer2D.cpp
enum class ShaderType
{
	BASIC,
	UI,
	GRADIENT,
	WATER
};

std::string ShaderTypeToString(ShaderType type);

#ifdef DEBUG_UNIFORMS
#include <typeinfo> // for typeid

template<typename... T>
static void PrintIncomingVariables(ShaderType shaderType, T&&... args)
{
	std::cout << ShaderTypeToString(shaderType) << ": " << std::endl;
	((std::cout << "Type T: " << typeid(T).name()), ...);
	std::cout << "." << std::endl;
}
#else
template<typename... T>
static void PrintIncomingVariables(ShaderType shaderType, T&&... args){};
#endif // DEBUG_UNIFORMS

class Renderer2D
{
public:
	Renderer2D(Camera* camera, Camera* screenCamera);
	~Renderer2D();

	template<typename... Args>
	void Draw(BatchRenderer* batchRenderer, ShaderType shaderType, CameraType cameraType, Args&&... args)
	{
		Camera* camera = nullptr;

		switch (cameraType)
		{
			case CameraType::ORTHOGRAPHIC:
				camera = m_camera;
				break;
			case CameraType::SCREENSPACE: 
				camera = m_screenCamera;
				break;
			default: break;
		}

		Shader* shader = m_shaderMap[ShaderTypeToString(shaderType)];
		shader->Use();
		shader->SetCommonUniforms(camera);

		SetShaderUniforms(shader, shaderType, std::forward<Args>(args)...);

		glm::vec2 camPos         = camera->GetPosition();
		float distanceFromCenter = -camera->GetAspectRatio().x + 6;

		batchRenderer->Render(camPos, distanceFromCenter);
	}

private:
	ShaderType m_shaderType = ShaderType::BASIC;

	std::map<std::string, Shader*> m_shaderMap;
	Camera* m_camera       = nullptr;
	Camera* m_screenCamera = nullptr;

	template<typename... T>
	void SetShaderUniforms(Shader* shader, ShaderType shaderType, T&&... optionalParams) const;
};

template<typename... T>
void Renderer2D::SetShaderUniforms(Shader* shader, ShaderType shaderType, T&&... optionalParams) const
{
	PrintIncomingVariables(shaderType, std::forward<T>(optionalParams)...);

	// Depending on the shader type, set the specific uniforms using the provided arguments.
	switch (shaderType)
	{
	case ShaderType::BASIC:
	{
		// Set suncolour and watercolour.
		(shader->SetUniform("SunColor", std::forward<T>(optionalParams)), ...);
		(shader->SetUniform("WaterColor", std::forward<T>(optionalParams)), ...);
		break;
	}
	case ShaderType::UI:
	{
		// Set the UI shader uniforms.
		break;
	}
	case ShaderType::GRADIENT:
	{
		(shader->SetUniform("GradientAmount", std::forward<T>(optionalParams)), ...);
		break;
	}
	case ShaderType::WATER:
	{
		(shader->SetUniform("time", std::forward<T>(optionalParams)), ...);
		(shader->SetUniform("lightDirection", std::forward<T>(optionalParams)), ...);

		break;
	}
	default:
		// Handle other shader types or throw an error.
		assert(false && "Unknown ShaderType.");
		break;
	}
}