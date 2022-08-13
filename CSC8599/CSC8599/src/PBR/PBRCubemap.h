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
	unsigned int			id;

    // pbr: set up projection and view matrices for capturing data onto the 6 cubemap face directions
    glm::mat4				captureProjection;
	std::vector<glm::mat4>	captureViews;

	PBRCubemap();

	void LoadMap(std::string path);
	void Allocate(GLenum format1, GLenum format2, GLuint width, GLuint height, GLenum type);

	void Generate();
	void Bind();
	void Init();

	void Cleanup();

private:
	ArrayObject				VAO;
};

#endif
