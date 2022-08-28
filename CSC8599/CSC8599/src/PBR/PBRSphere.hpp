#ifndef PBRSphere
#define PBRSphere

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stb/stb_image.h>

#include <string>
#include <vector>

#include "../graphics/Shader.h"
#include "../graphics/buffer/FrameBuffer.hpp"

class PBRSphere
{
public:
	void Render()
	{
		if (VAO.val == 0)
		{

		}
	}

private:
	ArrayObject VAO;

};

#endif // !PBRSphere
