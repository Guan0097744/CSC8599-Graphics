#ifndef MODEL_H
#define MODEL_H

#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"

#include "../physics/Rigidbody.h"

class Model
{
public:
	RigidBody rb;
	glm::vec3 pos;
	glm::vec3 size;

	Model(glm::vec3 pos = glm::vec3(0.0f), glm::vec3 size = glm::vec3(1.0f), bool noTex = false);

	virtual void Init() {};
	void LoadModel(std::string path);
	void Render(Shader& shader, float dt, bool setModel = true);
	void Cleanup();

	std::string directory;
	std::vector<Mesh> meshes;
	std::vector<Texture> texsLoaded;
	bool noTex;

protected:

	void ProcessNode(aiNode* node, const aiScene* scene);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
	//std::vector<Texture> LoadTextures(aiMaterial* mat, aiTextureType type);
	std::vector<Texture> LoadTextures(aiMaterial* mat, aiTextureType type, const aiScene* scene);
};


#endif // !MODEL_H
