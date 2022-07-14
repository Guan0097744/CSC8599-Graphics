#include "Shader.h"

#include <stdio.h>
#include <fstream>

// stream containing default headers
std::stringstream Shader::defaultHeaders;

Shader::Shader() {}

Shader::Shader(bool includeDefaultHeader, const char* vertexShaderPath, const char* fragShaderPath, const char* geoShaderPath)
{
	Generate(includeDefaultHeader, vertexShaderPath, fragShaderPath, geoShaderPath);
}

void Shader::Generate(bool includeDefaultHeader, const char* vertexShaderPath, const char* fragShaderPath, const char* geoShaderPath)
{
	id = glCreateProgram();

	// Compile and attach shaders
	CompileAndAttach(id, includeDefaultHeader, vertexShaderPath, GL_VERTEX_SHADER);
	CompileAndAttach(id, includeDefaultHeader, fragShaderPath, GL_FRAGMENT_SHADER);
	CompileAndAttach(id, includeDefaultHeader, geoShaderPath, GL_GEOMETRY_SHADER);
	glLinkProgram(id);

	// Linking errors
	int success;
	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if (!success) {
		char* infoLog = (char*)malloc(512);
		glGetProgramInfoLog(id, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
}

void Shader::Activate()
{
	glUseProgram(id);
}

void Shader::Cleanup()
{
	glDeleteProgram(id);
}

void Shader::LoadIntoDefault(const char* filepath)
{
	char* fileContents = Shader::LoadShaderSrc(false, filepath);

	Shader::defaultHeaders << fileContents;

	free(fileContents);
}

void Shader::ClearDefault()
{
	Shader::defaultHeaders.clear();
}

char* Shader::LoadShaderSrc(bool includeDefaultHeader, const char* filePath)
{
	std::string fullPath = Shader::defaultDirectory + '/' + filePath;

	FILE* file = NULL;
	fopen_s(&file, fullPath.c_str(), "rb");
	if (!file) {
		std::cout << "Could not open " << filePath << std::endl;
		return NULL;
	}

	// Move cursor to the end
	fseek(file, 0L, SEEK_END);
	// Get length
	int len = ftell(file);
	// return to beginning
	fseek(file, 0, SEEK_SET);

	// Read
	char* ret = NULL;
	int cursor = 0;
	if (includeDefaultHeader) 
	{
		// Copy header and advance cursor to read into space after default header
		cursor = Shader::defaultHeaders.str().size();
		ret = (char*)malloc(cursor + len + 1);
		memcpy_s(ret, cursor + len + 1, Shader::defaultHeaders.str().c_str(), cursor);
	}
	else 
	{
		ret = (char*)malloc(len + 1);
	}

	// Read from file
	fread(ret + cursor, 1, len, file);
	ret[cursor + len] = 0; // terminator

	return ret;
}

GLuint Shader::CompileShader(bool includeDefaultHeader, const char* filePath, GLuint type)
{
	// Create shader from file
	GLuint ret = glCreateShader(type);
	GLchar* shader = LoadShaderSrc(includeDefaultHeader, filePath);
	glShaderSource(ret, 1, &shader, NULL);
	glCompileShader(ret);
	free(shader);

	// Catch compilation error
	int success;
	glGetShaderiv(ret, GL_COMPILE_STATUS, &success);
	if (!success) {
		char* infoLog = (char*)malloc(512);
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

void Shader::Set4Float(const std::string& name, aiColor4D color)
{
	glUniform4f(glGetUniformLocation(id, name.c_str()), color.r, color.g, color.b, color.a);
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

void Shader::CompileAndAttach(GLuint id, bool includeDefaultHeader, const char* path, GLuint type)
{
	if (!path) {
		return;
	}

	GLuint shader = Shader::CompileShader(includeDefaultHeader, path, type);
	glAttachShader(id, shader);
	glDeleteShader(shader);
}
