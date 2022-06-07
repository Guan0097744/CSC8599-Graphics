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
	Texture(std::string dir, std::string path, aiTextureType type);

	void Generate();
	void Load(bool flip = true);

	void Bind();

	unsigned int GetId()		{ return id; }
	aiTextureType GetTexType()	{ return type; }
	std::string GetDir()		{ return dir; }
	std::string GetPath()		{ return path; }

private:
	unsigned int	id;
	aiTextureType	type;
	std::string		dir;	// Directory
	std::string		path;
};

#endif
