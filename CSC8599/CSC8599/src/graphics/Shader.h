#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <string>
#include <sstream>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/scene.h>

class Shader 
{
public:
	unsigned int id;	// Program ID

	Shader();

	// Initialize with paths to vertex, fragment, and optional geometry shaders
	Shader(bool includeDefaultHeader,
		const char* vertexShaderPath,
		const char* fragShaderPath,
		const char* geoShaderPath = nullptr);

	// generate using vertex, frag, and optional geometry shaders
	void Generate(bool includeDefaultHeader,
		const char* vertexShaderPath,
		const char* fragShaderPath,
		const char* geoShaderPath = nullptr);

	// Activate shader
	void Use();

	// Clean up
	void Cleanup();

	//============================================================================================//
	//static
	//============================================================================================//

	// Default directory
	static std::string defaultDirectory;

	// Stream containing default header source
	static std::stringstream defaultHeaders;

	// Load into default header
	static void LoadIntoDefault(const char* filepath);

	// Clear default header (after shader compilation)
	static void ClearDefault();

	// Load string from file
	static char* LoadShaderSrc(bool includeDefaultHeader, const char* filePath);

	// Compile shader program
	static GLuint CompileShader(bool includeDefaultHeader, const char* filePath, GLuint type);

	// Set uniform variables
	void SetBool(const std::string& name, bool value);
	void SetInt(const std::string& name, int value);
	void SetFloat(const std::string& name, float value);
	void Set3Float(const std::string& name, float v1, float v2, float v3);
	void Set3Float(const std::string& name, glm::vec3 v);
	void Set4Float(const std::string& name, float v1, float v2, float v3, float v4);
	void Set4Float(const std::string& name, aiColor4D color);
	void Set4Float(const std::string& name, glm::vec4 v);
	void SetMat3(const std::string& name, glm::mat3 val);
	void SetMat4(const std::string& name, glm::mat4 val);

	//============================================================================================//
	//Bindless Texture
	//============================================================================================//

	void SetHandle(const std::string& name, GLuint64 val);
	void SetHandleArray(const std::string& name, GLsizei count​, GLuint64* val​);

private:
	void CompileAndAttach(GLuint id, bool includeDefaultHeader, const char* path, GLuint type);
};

#endif