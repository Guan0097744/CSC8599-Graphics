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

	void SetBuffer();
	void LoadMap();

	void Generate();
	void Bind();
	void Init();

private:
	FramebufferObject captureFBO;

};



#endif
