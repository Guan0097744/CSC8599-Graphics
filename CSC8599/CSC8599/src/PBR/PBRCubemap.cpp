#include "PBRCubemap.h"

PBRCubemap::PBRCubemap()
{

}

void PBRCubemap::SetBuffer()
{
	//captureFBO;
}

void PBRCubemap::LoadMap(std::string path)
{
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrComponents;
    float* data = stbi_loadf(path.c_str(), &width, &height, &nrComponents, 0);
    unsigned int hdrTexture;
    if (data)
    {
        glGenTextures(1, &hdrTexture);
        glBindTexture(GL_TEXTURE_2D, hdrTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data); // note how we specify the texture's data value to be float

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Failed to load HDR image." << std::endl;
    }
}

void PBRCubemap::Allocate(GLenum format, GLuint width, GLuint height, GLenum type)
{

}

void PBRCubemap::Generate()
{
	glGenTextures(1, &id);
}

void PBRCubemap::Bind()
{

}

void PBRCubemap::Init()
{

}

void PBRCubemap::Cleanup()
{

}
