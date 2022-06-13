#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stb/stb_image.h>

#include <assimp/scene.h>

class Texture
{
public:
	Texture(std::string dir, std::string path, aiTextureType type);

	void Generate();
	void Load(bool flip = true, GLint wrapMode = GL_REPEAT, GLint magFilterMode = GL_LINEAR, GLint minFilterMode = GL_LINEAR_MIPMAP_LINEAR);
	void LoadFromAssimp(const aiTexture* aiTex, bool flip = false, GLint wrapMode = GL_REPEAT, GLint magFilterMode = GL_LINEAR, GLint minFilterMode = GL_LINEAR_MIPMAP_LINEAR);

	void Bind();

	unsigned int GetId()			{ return id; }
	void SetId(unsigned int texId)	{ id = texId; }

	aiTextureType GetTexType()		{ return type; }
	std::string GetDir()			{ return dir; }
	std::string GetPath()			{ return path; }

private:
	unsigned int	id;
	aiTextureType	type;
	std::string		dir;	// Directory
	std::string		path;
};

#endif
