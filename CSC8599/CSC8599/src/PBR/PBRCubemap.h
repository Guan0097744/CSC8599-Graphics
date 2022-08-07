#ifndef PBRCUBEMAP_H
#define PBRCUBEMAP_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stb/stb_image.h>

#include <string>
#include <vector>

#include "../graphics/Shader.h"
#include "../graphics/buffer/FrameBuffer.hpp"

class PBRCubemap
{
public:
	unsigned int id;

	PBRCubemap();

	void SetBuffer();
	void LoadMap(std::string path);
	void Allocate(GLenum format, GLuint width, GLuint height, GLenum type);

	void Generate();
	void Bind();
	void Init();

	void Cleanup();

private:
	FramebufferObject captureFBO;

};



#endif
