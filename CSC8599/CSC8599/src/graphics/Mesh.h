#ifndef MESH_H
#define MESH_H

#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "Shader.h"
#include "Texture.h"

struct Vertex
{
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 texCoord;

	static std::vector<Vertex> GenList(float* vertices, int numVertices);
};

class Mesh
{
public:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	// Material diffuse value
	aiColor4D diffuse;

	// Material specular value
	aiColor4D specular;

	unsigned int VAO;

	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures = {});
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, aiColor4D diffuse, aiColor4D specular);

	void Render(Shader& shader);

	void Cleanup();

private:
	unsigned int VBO, EBO;
	bool noTex;

	void Setup();
};

#endif // !MESH_H
