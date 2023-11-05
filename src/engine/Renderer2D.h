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
#include <map>
#include "BatchRenderer.h"

class BatchRenderer;

// If you add a shader here, make sure to add it to ShaderTypeToString() in Renderer2D.cpp
enum class ShaderType
{
	BASIC,
	UI,
	GRADIENT
};

std::string ShaderTypeToString(ShaderType type);

class Renderer2D
{
public:
	Renderer2D(Camera* camera);
	~Renderer2D();

	template <typename... Args>
	void Draw(BatchRenderer* batchRenderer, ShaderType shaderType, Args&& ... args)
	{
		Shader* shader = m_shaderMap[ShaderTypeToString(shaderType)];
		shader->Use();
		shader->SetCommonUniforms(m_camera);
		SetShaderUniforms(shader, shaderType, std::forward<Args>(args)...);

		glm::vec2 camPos = m_camera->GetPosition();
		float distanceFromCenter = -m_camera->GetAspectRatio().x + 6;

		batchRenderer->Render(camPos, distanceFromCenter);
	}

	Shader* GetShader(const char* shaderName);
	Shader* GetShader(ShaderType shaderType);
private:
	void AddTextureSlotsToShader(Shader* shader);

	ShaderType m_shaderType = ShaderType::BASIC;

	std::map<std::string, Shader*> m_shaderMap;
	Camera* m_camera = nullptr;

// SPECIALIZATIONS
	void SetShaderUniforms(Shader* shader, ShaderType shaderType) const { /* No arguments */ }
	void SetShaderUniforms(Shader* shader, ShaderType shaderType, float gradientAmount) const {
		// Depending on the shader type, set the specific uniforms using the provided arguments.
		switch (shaderType) {
		case ShaderType::BASIC:
		{
			break;
		}
		case ShaderType::UI:
		{
			// Set the UI shader uniforms.
			break;
		}
		case ShaderType::GRADIENT:
		{
			shader->SetUniform("GradientAmount", gradientAmount);
			break;
		}
		default:
			// Handle other shader types or throw an error.
			assert(false && "Unknown ShaderType.");
			break;

		}
	}

	void SetShaderUniforms(Shader* shader, ShaderType shaderType, glm::vec4 sunColour) const {
		// Depending on the shader type, set the specific uniforms using the provided arguments.
		switch (shaderType) {
		case ShaderType::BASIC:
		{
			shader->SetUniform("SunColor", sunColour);
		}
		case ShaderType::UI:
		{
			// Set the UI shader uniforms.
			break;
		}
		case ShaderType::GRADIENT:
		{
			break;
		}
		default:
			// Handle other shader types or throw an error.
			assert(false && "Unknown ShaderType.");
			break;

		}
	}
};
