#pragma once
#include "glew.h"
#include "glfw3.h"
#include "glm.hpp"
#include <string>

class Shader
{
public:
	Shader(std::string name, const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);
	Shader(std::string name);
	Shader() = default;
	~Shader();

	unsigned int CompileShader(unsigned int type, const std::string& source);
	void CheckCompileErrors(GLuint shader, std::string type);
	unsigned int GetID();
	void Use();
	std::string GetName();;

#pragma region Uniform functions
	// ------------------------------------------------------------------------
	void setBool(const std::string& name, bool value) const;

	// ------------------------------------------------------------------------
	void setInt(const std::string& name, int value) const;

	// ------------------------------------------------------------------------
	void setFloat(const std::string& name, float value) const;

	// ------------------------------------------------------------------------
	void setVec2(const std::string& name, const glm::vec2& value) const;
	void setVec2(const std::string& name, float x, float y) const;

	// ------------------------------------------------------------------------
	void setVec3(const std::string& name, const glm::vec3& value) const;
	void setVec3(const std::string& name, float x, float y, float z) const;

	// ------------------------------------------------------------------------
	void setVec4(const std::string& name, const glm::vec4& value) const;
	void setVec4(const std::string& name, float x, float y, float z, float w);

	// ------------------------------------------------------------------------
	void setMat2(const std::string& name, const glm::mat2& mat) const;

	// ------------------------------------------------------------------------
	void setMat3(const std::string& name, const glm::mat3& mat) const;

	// ------------------------------------------------------------------------
	void setMat4(const std::string& name, const glm::mat4& mat) const;
#pragma endregion

protected:
	unsigned int m_shaderId = 0;
	std::string m_name = "DefaultName";
};
