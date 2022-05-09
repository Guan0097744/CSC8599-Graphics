#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <string>
#include <sstream>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Shader 
{
public:
	unsigned int id;	// Program ID

	Shader();

	// Initialize with paths to vertex, fragment, and optional geometry shaders
	Shader(const char* vertexShaderPath,
		const char* fragShaderPath);

	// Activate shader
	void Activate();

	// Clean up
	void CleanUp();

	// Load string from file
	static std::string LoadShaderSrc(const char* filePath);

	// Compile shader program
	static GLuint CompileShader(const char* filePath, GLuint type);

	// Load into default header
	static void LoadIntoDefault(const char* filepath);

	// Clear default header (after shader compilation)
	static void ClearDefault();

	// Default directory
	static std::string defaultDirectory;

	// Stream containing default header source
	static std::stringstream defaultHeaders;

	// Set uniform variables
	void SetMat4(const std::string& name, glm::mat4 val);
};

#endif