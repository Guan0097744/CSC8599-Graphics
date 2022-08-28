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
#include "../physics/CollisionModel.h"
#include "../algorithms/Bounds.h"

// Model switches
#define DYNAMIC				(unsigned int)1 // 0b00000001
#define CONST_INSTANCES		(unsigned int)2 // 0b00000010
#define NO_TEX				(unsigned int)4	// 0b00000100

// Forward declaration
class Scene;

class Model
{
public:
	std::string					id;						// id of model in scene
	std::vector<Mesh>			meshes;					// list of meshes
	CollisionModel*				collision;				// pointer to the collision model
	std::vector<BoundingRegion> boundingRegions;		// list of bounding regions (1 for each mesh)
	std::vector<RigidBody*>		instances;				// list of instances

	unsigned int				maxNumInstances;		// maximum number of instances
	unsigned int				currentNumInstances;	// current number of instances
	unsigned int				switches;				// combination of switches above

	Model(std::string id, unsigned int maxNumInstances, unsigned int flags = 0);

	virtual void Init() {};
	virtual void Render(Shader& shader, float dt);

	void BindlessTexture(Shader& shader);

	void LoadModel(std::string path);
	void EnableCollisionModel();
	void AddMesh(Mesh* mesh);
	void Cleanup();

	RigidBody* GenerateInstance(glm::vec3 size, float mass, glm::vec3 pos, glm::vec3 rot);
	void InitInstances();
	void RemoveInstance(unsigned int idx);
	void RemoveInstance(std::string instanceId);
	unsigned int GetIdx(std::string id);

protected:
	std::string					directory;
	std::vector<Texture>		texsLoaded;
	//bool						noTex;

	//============================================================================================//
	//VBOs for model matrices
	//============================================================================================//

	BufferObject				modelVBO;
	BufferObject				normalModelVBO;

	void ProcessNode(aiNode* node, const aiScene* scene);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
	Mesh ProcessMesh(BoundingRegion br,
		unsigned int numVertices, float* vertices,
		unsigned int numIndices, unsigned int* indices,
		bool calcTanVectors = true,
		unsigned int numCollisionPoints = 0, float* collisionPoints = NULL,
		unsigned int numCollisionFaces = 0, unsigned int* collisionIndices = NULL,
		bool pad = false);	// proces a custom mesh

	//std::vector<Texture> LoadTextures(aiMaterial* mat, aiTextureType type);
	std::vector<Texture> LoadTextures(aiMaterial* mat, aiTextureType type, const aiScene* scene);
};


#endif // !MODEL_H
