#pragma once
#include "BatchRenderer.h"
#include "Camera.h"
#include "GameObject.h"
#include "Shader.h"
#include "Texture.h"
#include <array>
#include <glm.hpp>
#include <map>
#include <memory>
#include <string>
#include <vector>

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

	template<typename... Args>
	void Draw(BatchRenderer* batchRenderer, ShaderType shaderType, Args&&... args)
	{
		Shader* shader = m_shaderMap[ShaderTypeToString(shaderType)];
		shader->Use();
		shader->SetCommonUniforms(m_camera);
		SetShaderUniforms(shader, shaderType, std::forward<Args>(args)...);

		glm::vec2 camPos         = m_camera->GetPosition();
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

	template<typename... T>
	void SetShaderUniforms(Shader* shader, ShaderType shaderType, T&&... optionalParams) const
	{
		// Depending on the shader type, set the specific uniforms using the provided arguments.
		switch (shaderType)
		{
		case ShaderType::BASIC:
		{
			if constexpr ((std::is_same_v<T, glm::vec4> || ...))
			{ // Unpack template parameters with fold expression
				(shader->SetUniform("SunColor", std::forward<T>(optionalParams)), ...);
			}
			break;
		}
		case ShaderType::UI:
		{
			// Set the UI shader uniforms.
			break;
		}
		case ShaderType::GRADIENT:
		{
			if constexpr ((std::is_same_v<T, float> || ...))
			{
				(shader->SetUniform("GradientAmount", std::forward<T>(optionalParams)), ...);
			}
			break;
		}
		default:
			// Handle other shader types or throw an error.
			assert(false && "Unknown ShaderType.");
			break;
		}
	}
};
