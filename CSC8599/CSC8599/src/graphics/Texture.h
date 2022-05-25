#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stb/stb_image.h>

class Texture
{
public:
	Texture();
	Texture(const char* path, const char* name, bool defaultParametres = true);

	void Generate();
	void Load(bool flip = true);

	void SetFilters(GLenum all);
	void SetFilters(GLenum mag, GLenum min);

	void SetWrap(GLenum all);
	void SetWrap(GLenum s, GLenum t);

	void Bind();
	
	unsigned int id;	// Texture id
	unsigned int tex;
	const char* name;

private:
	static int currentId;

	const char* path;
	int width;
	int height;
	int nChannels;

};

#endif
