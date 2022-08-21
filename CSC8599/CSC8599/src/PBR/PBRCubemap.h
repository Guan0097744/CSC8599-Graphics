#ifndef PBRCUBEMAP_H
#define PBRCUBEMAP_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stb/stb_image.h>

#include <string>
#include <vector>

#include "../graphics/Shader.h"
#include "../graphics/buffer/FrameBuffer.hpp"

class PBRScene;

class PBRCubemap
{
public:
	unsigned int			id;

	PBRCubemap();

	void Allocate(GLenum format1, GLenum format2, GLuint width, GLuint height, GLenum type);
	void Generate();
	void Bind();
	void Init();
	void DrawVAO();

	void Cleanup();

private:
	ArrayObject				VAO;
};

#endif
