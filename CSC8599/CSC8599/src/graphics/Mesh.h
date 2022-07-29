#ifndef MESH_H
#define MESH_H

#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include "Shader.h"
#include "Texture.h"
#include "Material.h"
#include "buffer/VertexBuffer.hpp"
#include "models/Box.hpp"
#include "../algorithms/Bounds.h"
#include "../physics/CollisionMesh.h"

struct Vertex
{
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 texCoord;
	glm::vec3 tangent;

	static std::vector<Vertex> GenList(float* vertices, int numVertices);
	static void AverageVectors(glm::vec3& baseVec, glm::vec3 addition, unsigned char existingContributions);
	static void CalcTanVectors(std::vector<Vertex>& list, std::vector<unsigned int>& indices);
};

class Mesh
{
public:
	BoundingRegion				br;			// Bounding region for mesh
	CollisionMesh*				collision;	// Pointer to the attached collision mesh

	std::vector<Vertex>			vertices;
	std::vector<unsigned int>	indices;
	std::vector<Texture>		textures;

	ArrayObject					VAO;		// Vertex array object pointing to all data for the mesh

	aiColor4D					diffuse;	// Material diffuse value
	aiColor4D					specular;	// Material specular value

	Mesh();
	Mesh(BoundingRegion br);
	Mesh(BoundingRegion br, std::vector<Texture> textures);
	Mesh(BoundingRegion br, aiColor4D diff, aiColor4D spec);
	Mesh(BoundingRegion br, Material m);
	Mesh(std::vector<Texture> textures);

	void LoadData(std::vector<Vertex> vertices, std::vector<unsigned int> indices, bool pad = false);
	void LoadCollisionMesh(unsigned int noPoints, float* coordinates, unsigned int noFaces, unsigned int* indices);
	void SetupTextures(std::vector<Texture> textures);
	void SetupColors(aiColor4D diff, aiColor4D spec);
	void SetupMaterial(Material mat);

	void Render(Shader& shader, unsigned int numInstances);
	void Cleanup();

private:
	bool noTex;

	void Setup();
};

#endif // !MESH_H
