#include "Texture.h"

#include <iostream>


Texture::Texture(std::string dir, std::string path, aiTextureType type) :
	dir(dir), path(path), type(type)
{
	Generate();
}


void Texture::Generate()
{
	glGenTextures(1, &id);
}

void Texture::Load(bool flip, GLint wrapMode, GLint magFilterMode, GLint minFilterMode)
{
	stbi_set_flip_vertically_on_load(flip);

	int width, height, nChannels;
	unsigned char* data = stbi_load((dir + "/" + path).c_str(), &width, &height, &nChannels, 0);

	GLenum colorMode = GL_RGB;
	switch (nChannels)
	{
	case 1:
		colorMode = GL_RED;
		break;
	case 4:
		colorMode = GL_RGBA;
		break;
	}

	if(data)
	{
		glBindTexture(GL_TEXTURE_2D, id);
		glTexImage2D(GL_TEXTURE_2D, 0, colorMode, width, height, 0, colorMode, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		// Set wraps
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);

		// Set filters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilterMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilterMode);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
}

void Texture::LoadFromAssimp(const aiTexture* aiTex, GLint wrapMode, GLint magFilterMode, GLint minFilterMode)
{
	if (aiTex == nullptr)
		return;

	int width, height, nChannels;
	unsigned char* image_data = nullptr;
	if (aiTex->mHeight == 0)
	{
		image_data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(aiTex->pcData), aiTex->mWidth, &width, &height, &nChannels, 0);
	}
	else
	{
		image_data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(aiTex->pcData), aiTex->mWidth * aiTex->mHeight, &width, &height, &nChannels, 0);
	}

	GLenum colorMode = GL_RGB;
	switch (nChannels)
	{
	case 1:
		colorMode = GL_RED;
		break;
	case 4:
		colorMode = GL_RGBA;
		break;
	}

	if (image_data)
	{
		glBindTexture(GL_TEXTURE_2D, id);
		glTexImage2D(GL_TEXTURE_2D, 0, colorMode, width, height, 0, colorMode, GL_UNSIGNED_BYTE, image_data);
		glGenerateMipmap(GL_TEXTURE_2D);

		// Set wraps
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);

		// Set filters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilterMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilterMode);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(image_data);

}

void Texture::Bind()
{
	glBindTexture(GL_TEXTURE_2D, id);
}
