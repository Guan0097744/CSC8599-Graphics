#include "Shader.h"

#include <stdio.h>
#include <fstream>

Shader::Shader()
{

}

Shader::Shader(const char* vertexShaderPath, const char* fragShaderPath)
{
	int success;
	char infoLog[512];

	GLuint vertexShader = CompileShader(vertexShaderPath, GL_VERTEX_SHADER);
	GLuint fragShader = CompileShader(fragShaderPath, GL_FRAGMENT_SHADER);

	id = glCreateProgram();
	glAttachShader(id, vertexShader);
	glAttachShader(id, fragShader);
	glLinkProgram(id);

	// Error catch
	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(id, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragShader);
}

void Shader::Activate()
{
	glUseProgram(id);
}

void Shader::CleanUp()
{
	glDeleteProgram(id);
}

std::string Shader::LoadShaderSrc(const char* filePath)
{
	std::ifstream file;
	std::stringstream buf;
	std::string ret = "";

	file.open(filePath);
	if (file.is_open())
	{
		buf << file.rdbuf();
		ret = buf.str();
	}
	else
	{
		std::cout << "Could not open " << filePath << std::endl;
	}

	file.close();

	return ret;
}

GLuint Shader::CompileShader(const char* filePath, GLuint type)
{
	int success;
	char infoLog[512];

	GLuint ret = glCreateShader(type);
	std::string shaderSrc = LoadShaderSrc(filePath);
	const GLchar* shader = shaderSrc.c_str();
	glShaderSource(ret, 1, &shader, NULL);
	glCompileShader(ret);
	// Error catch
	glGetShaderiv(ret, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(ret, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::" << filePath << "::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	return ret;
}

void Shader::SetBool(const std::string& name, bool value)
{
	glUniform1i(glGetUniformLocation(id, name.c_str()), (int)value);
}

void Shader::SetInt(const std::string& name, int value)
{
	glUniform1i(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::SetFloat(const std::string& name, float value)
{
	glUniform1f(glGetUniformLocation(id, name.c_str()), value);
}

void Shader::Set3Float(const std::string& name, float v1, float v2, float v3)
{
	glUniform3f(glGetUniformLocation(id, name.c_str()), v1, v2, v3);
}

void Shader::Set3Float(const std::string& name, glm::vec3 v)
{
	glUniform3f(glGetUniformLocation(id, name.c_str()), v.x, v.y, v.z);
}

void Shader::Set4Float(const std::string& name, float v1, float v2, float v3, float v4)
{
	glUniform4f(glGetUniformLocation(id, name.c_str()), v1, v2, v3, v4);
}

void Shader::Set4Float(const std::string& name, glm::vec4 v)
{
	glUniform4f(glGetUniformLocation(id, name.c_str()), v.x, v.y, v.z, v.w);
}

void Shader::SetMat3(const std::string& name, glm::mat3 val)
{
	glUniformMatrix3fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(val));
}

void Shader::SetMat4(const std::string& name, glm::mat4 val)
{
	glUniformMatrix4fv(glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(val));
}