#ifndef CUBEMAP_H
#define CUBEMAP_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stb/stb_image.h>

#include <string>
#include <vector>

#include "Shader.h"
#include "buffer/VertexBuffer.hpp"

class Scene;

class Cubemap 
{
public:
	// texture object
	unsigned int id;

	Cubemap();

	void Generate();

	void Bind();

	void LoadTextures(std::string dir,
		std::string right	= "right.png",
		std::string left	= "left.png",
		std::string top		= "top.png",
		std::string bottom	= "bottom.png",
		std::string front	= "front.png",
		std::string back	= "back.png");

	void Allocate(GLenum format, GLuint width, GLuint height, GLenum type);

	void Init();

	void Render(Shader shader, Scene* scene);

	void Cleanup();

private:
	std::string					dir;
	std::vector<std::string>	faces;
	bool						hasTextures;

	// Cube object
	ArrayObject					VAO;
};

#endif