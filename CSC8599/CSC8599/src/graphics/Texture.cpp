#include "Texture.h"

#include <iostream>


Texture::Texture()
{
	//Generate();
}

Texture::Texture(std::string name)
	:name(name),type(aiTextureType_NONE)
{
	Generate();
}

Texture::Texture(std::string dir, std::string path, aiTextureType type) :
	dir(dir), path(path), type(type)
{
	Generate();
}

Texture::Texture(std::string dirPath, aiTextureType type)
{
	dir = dirPath.substr(0, dirPath.find_last_of("/"));
	path = dirPath.substr(dir.length(), dirPath.length() - 1);
	std::cout << "DIR: " << dir << ". PATH: " << path << std::endl;
	Generate();
}

void Texture::Generate()
{
	glGenTextures(1, &id);
}

void Texture::Load(bool flip)
{
	stbi_set_flip_vertically_on_load(flip);

	int width, height, nChannels;

	std::cout << "Texture data: " << (dir + "/" + path) << std::endl;
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
	};

	if (data) 
	{
		
		glBindTexture(GL_TEXTURE_2D, id);
		glTexImage2D(GL_TEXTURE_2D, 0, colorMode, width, height, 0, colorMode, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else 
	{
		std::cout << "Failed to load texture. Image not loaded at " << path << std::endl;
	}

	stbi_image_free(data);
}

void Texture::LoadFromAssimp(const aiTexture* aiTex, bool flip)
{
	if (aiTex == nullptr)
		return;

	stbi_set_flip_vertically_on_load(flip);
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
	};

	if (image_data)
	{
		glBindTexture(GL_TEXTURE_2D, id);
		glTexImage2D(GL_TEXTURE_2D, 0, colorMode, width, height, 0, colorMode, GL_UNSIGNED_BYTE, image_data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		std::cout << "Failed to load texture. Image not loaded at " << path << std::endl;
	}
	stbi_image_free(image_data);

}

void Texture::LoadHDR(std::string dirPath)
{
	stbi_set_flip_vertically_on_load(true);
	int width, height, nrComponents;
	float* data = stbi_loadf(dirPath.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		glBindTexture(GL_TEXTURE_2D, id);
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

void Texture::Allocate(GLenum format, GLuint width, GLuint height, GLenum type)
{
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, type, NULL);
}

void Texture::Allocate(GLenum format1, GLenum format2, GLuint width, GLuint height, GLenum type)
{
	glTexImage2D(GL_TEXTURE_2D, 0, format1, width, height, 0, format2, type, NULL);
}

void Texture::SetParams(GLenum texMinFilter, GLenum texMagFilter, GLenum wrapS, GLenum wrapT)
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texMinFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texMagFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
}

void Texture::Bind()
{
	glBindTexture(GL_TEXTURE_2D, id);
}

void Texture::Cleanup()
{
	glDeleteTextures(1, &id);
}

void Texture::Bindless()
{
	glBindTexture(GL_TEXTURE_2D, id);
	handle = glGetTextureHandleARB(id);
	glMakeTextureHandleResidentARB(handle);
}
