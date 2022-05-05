#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

#include <string>
#include <sstream>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

/*
	Class to represent shader program
*/

class Shader 
{
public:
	// program ID
	unsigned int ID;

	/*
		constructors
	*/

	// default
	Shader();

	// initialize with paths to vertex, fragment, and optional geometry shaders
	Shader(bool includeDefaultHeader,
		const char* vertexShaderPath,
		const char* fragShaderPath,
		const char* geoShaderPath = nullptr);

	/*
		process functions
	*/

	// generate using vertex, frag, and optional geometry shaders
	void Generate(bool includeDefaultHeader,
		const char* vertexShaderPath,
		const char* fragShaderPath,
		const char* geoShaderPath = nullptr);

	// activate shader
	void Activate();

	// cleanup
	void Cleanup();

	/*
		set uniform variables
	*/

	void SetBool(const std::string& name, bool value);
	void SetInt(const std::string& name, int value);
	void SetFloat(const std::string& name, float value);
	void Set3Float(const std::string& name, float v1, float v2, float v3);
	void Set3Float(const std::string& name, glm::vec3 v);
	void Set4Float(const std::string& name, float v1, float v2, float v3, float v4);
	//void set4Float(const std::string& name, aiColor4D color);
	void Set4Float(const std::string& name, glm::vec4 v);
	void SetMat3(const std::string& name, glm::mat3 val);
	void SetMat4(const std::string& name, glm::mat4 val);

	/*
		static
	*/

	// compile shader program
	static GLuint CompileShader(bool includeDefaultHeader, const char* filePath, GLuint type);

	// default directory
	static std::string defaultDirectory;

	// stream containing default header source
	static std::stringstream defaultHeaders;

	// load into default header
	static void LoadIntoDefault(const char* filepath);

	// clear default header (after shader compilation)
	static void ClearDefault();

	// load string from file
	static char* LoadShaderSrc(bool includeDefaultHeader, const char* filePath);
};

#endif