#pragma once
#include "glew.h"
#include "glfw3.h"
#include "glm.hpp"
#include <string>

class Camera;

class Shader
{
public:
	Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);
	Shader(std::string_view vertexPath, std::string_view fragmentPath, std::string_view geometryPath);
	Shader() = default;
	~Shader();

	void CreateShader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);

	unsigned int CompileShader(unsigned int type, const std::string& source);
	void CheckCompileErrors(GLuint shader, std::string type);
	unsigned int GetID();
	void Use();

	void SetCommonUniforms(Camera* camera);

#pragma region Uniform functions
	template <typename T>
    void SetUniform(const std::string& name, T value) const {
        assert(true && "Uniform variable not found in the shader.");
    }

    // Specializations for supported types
    template <>
    void SetUniform<bool>(const std::string& name, bool value) const {
        glUniform1i(glGetUniformLocation(m_shaderId, name.c_str()), static_cast<int>(value));
    }

    template <>
    void SetUniform<int>(const std::string& name, int value) const {
        glUniform1i(glGetUniformLocation(m_shaderId, name.c_str()), value);
    }

    template <>
    void SetUniform<float>(const std::string& name, float value) const {
        glUniform1f(glGetUniformLocation(m_shaderId, name.c_str()), value);
    }

    template <>
    void SetUniform<glm::vec2>(const std::string& name, glm::vec2 value) const {
        glUniform2fv(glGetUniformLocation(m_shaderId, name.c_str()), 1, &value[0]);
    }

    template <>
    void SetUniform<glm::vec3>(const std::string& name, glm::vec3 value) const {
        glUniform3fv(glGetUniformLocation(m_shaderId, name.c_str()), 1, &value[0]);
    }

    template <>
    void SetUniform<glm::vec4>(const std::string& name, glm::vec4 value) const {
        glUniform4fv(glGetUniformLocation(m_shaderId, name.c_str()), 1, &value[0]);
    }

    template <>
    void SetUniform<glm::mat2>(const std::string& name, glm::mat2 mat) const {
        glUniformMatrix2fv(glGetUniformLocation(m_shaderId, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    template <>
    void SetUniform<glm::mat3>(const std::string& name, glm::mat3 mat) const {
        glUniformMatrix3fv(glGetUniformLocation(m_shaderId, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    template <>
    void SetUniform<glm::mat4>(const std::string& name, glm::mat4 mat) const {
        glUniformMatrix4fv(glGetUniformLocation(m_shaderId, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
#pragma endregion

protected:
	unsigned int m_shaderId = 0;
};
