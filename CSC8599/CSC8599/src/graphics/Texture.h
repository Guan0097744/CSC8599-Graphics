#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stb/stb_image.h>

#include <assimp/scene.h>

class Texture
{
public:
	Texture();
	Texture(std::string name);
	Texture(std::string dir, std::string path, aiTextureType type);
	Texture(std::string dirPath, aiTextureType type);

	void Generate();

	void Load(bool flip = true);
	void LoadFromAssimp(const aiTexture* aiTex, bool flip = false);
	void LoadHDR(std::string path);

	void Allocate(GLenum format, GLuint width, GLuint height, GLenum type);

	static void SetParams(GLenum texMinFilter = GL_NEAREST,
		GLenum texMagFilter = GL_NEAREST,
		GLenum wrapS = GL_REPEAT,
		GLenum wrapT = GL_REPEAT);

	void Bind();
	void Cleanup();

	//============================================================================================//
	//Variables
	//============================================================================================//

	unsigned int	id;
	aiTextureType	type;
	std::string		name;
	std::string		dir;	// Directory
	std::string		path;
};

#endif
