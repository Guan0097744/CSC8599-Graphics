#include "Model.h"

#include <iostream>
#include <limits>

#include "../physics/Environment.h"
#include "../Scene.h"

Model::Model(std::string id, unsigned int maxNumInstances, unsigned int flags)
	: id(id), switches(flags),
	currentNumInstances(0), maxNumInstances(maxNumInstances), instances(maxNumInstances),
	collision(nullptr) {}

void Model::LoadModel(std::string path)
{
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl << "Could not load model at " << path << std::endl;
		return;
	}
	directory = path.substr(0, path.find_last_of("/"));

	ProcessNode(scene->mRootNode, scene);
}

void Model::EnableCollisionModel()
{
	if (!this->collision) 
	{
		this->collision = new CollisionModel(this);
	}
}

void Model::AddMesh(Mesh* mesh)
{
	meshes.push_back(*mesh);
	boundingRegions.push_back(mesh->br);
}

void Model::Render(Shader& shader, float dt)
{
	if (!States::IsActive(&switches, CONST_INSTANCES)) 
	{
		// dynamic instances - update VBO data

		// create list of each
		std::vector<glm::mat4> models(currentNumInstances);
		std::vector<glm::mat3> normalModels(currentNumInstances);

		// determine if instances are moving
		bool doUpdate = States::IsActive(&switches, DYNAMIC);

		// iterate through each instance
		for (int i = 0; i < currentNumInstances; i++) 
		{
			if (doUpdate) {
				// update Rigid Body
				instances[i]->Update(dt);
				// activate moved switch
				States::Activate(&instances[i]->state, INSTANCE_MOVED);
			}
			else {
				// deactivate moved switch
				States::Deactivate(&instances[i]->state, INSTANCE_MOVED);
			}

			// add updated matrices
			models[i] = instances[i]->model;
			normalModels[i] = instances[i]->normalModel;
		}

		if (currentNumInstances) 
		{
			// set transformation data
			modelVBO.Bind();
			modelVBO.UpdateData<glm::mat4>(0, currentNumInstances, &models[0]);
			normalModelVBO.Bind();
			normalModelVBO.UpdateData<glm::mat3>(0, currentNumInstances, &normalModels[0]);
		}
	}

	// set shininess
	shader.SetFloat("material.shininess", 0.5f);

	// render each mesh
	for (unsigned int i = 0, numMeshes = meshes.size(); i < numMeshes; i++) 
	{
		meshes[i].Render(shader, currentNumInstances);
	}
}

void Model::Cleanup()
{
	// free all instances
	/*for (unsigned int i = 0, len = instances.size(); i < len; i++) 
	{
		if (instances[i]) 
		{
			free(instances[i]);
		}
	}*/
	instances.clear();

	// cleanup each mesh
	for (unsigned int i = 0, len = instances.size(); i < len; i++) 
	{
		meshes[i].Cleanup();
	}

	// free up memory for position and size VBOs
	modelVBO.Cleanup();
	normalModelVBO.Cleanup();
}

RigidBody* Model::GenerateInstance(glm::vec3 size, float mass, glm::vec3 pos, glm::vec3 rot)
{
	if (currentNumInstances >= maxNumInstances) 
	{
		// all slots filled
		return nullptr;
	}

	// instantiate new instance
	instances[currentNumInstances] = new RigidBody(id, size, mass, pos, rot);
	return instances[currentNumInstances++];
}

void Model::InitInstances()
{
	//============================================================================================//
	//Default values
	//============================================================================================//

	GLenum usage				= GL_DYNAMIC_DRAW;
	glm::mat4* modelData		= nullptr;
	glm::mat3* normalModelData	= nullptr;

	std::vector<glm::mat4> models(currentNumInstances);
	std::vector<glm::mat3> normalModels(currentNumInstances);

	if (States::IsActive(&switches, CONST_INSTANCES)) 
	{
		// instances won't change, set data pointers

		for (unsigned int i = 0; i < currentNumInstances; i++) 
		{
			models[i] = instances[i]->model;
			normalModels[i] = instances[i]->normalModel;
		}

		if (currentNumInstances) 
		{
			modelData = &models[0];
			normalModelData = &normalModels[0];
		}

		usage = GL_STATIC_DRAW;
	}

	// generate matrix VBOs
	modelVBO = BufferObject(GL_ARRAY_BUFFER);
	modelVBO.Generate();
	modelVBO.Bind();
	modelVBO.SetData<glm::mat4>(UPPER_BOUND, modelData, usage);

	normalModelVBO = BufferObject(GL_ARRAY_BUFFER);
	normalModelVBO.Generate();
	normalModelVBO.Bind();
	normalModelVBO.SetData<glm::mat3>(UPPER_BOUND, normalModelData, usage);

	// set attribute pointers for each mesh
	for (unsigned int i = 0, size = meshes.size(); i < size; i++) 
	{
		meshes[i].VAO.Bind();

		// Set vertex attrib pointers
		modelVBO.Bind();
		modelVBO.SetAttPointer<glm::vec4>(4, 4, GL_FLOAT, 4, 0, 1);
		modelVBO.SetAttPointer<glm::vec4>(5, 4, GL_FLOAT, 4, 1, 1);
		modelVBO.SetAttPointer<glm::vec4>(6, 4, GL_FLOAT, 4, 2, 1);
		modelVBO.SetAttPointer<glm::vec4>(7, 4, GL_FLOAT, 4, 3, 1);

		normalModelVBO.Bind();
		normalModelVBO.SetAttPointer<glm::vec3>(8, 3, GL_FLOAT, 3, 0, 1);
		normalModelVBO.SetAttPointer<glm::vec3>(9, 3, GL_FLOAT, 3, 1, 1);
		normalModelVBO.SetAttPointer<glm::vec3>(10, 3, GL_FLOAT, 3, 2, 1);

		ArrayObject::Clear();
	}
}

void Model::RemoveInstance(unsigned int idx)
{
	if (idx < maxNumInstances) 
	{
		// shift down
		for (unsigned int i = idx + 1; i < currentNumInstances; i++) 
		{
			instances[i - 1] = instances[i];
		}
		currentNumInstances--;
	}
}

void Model::RemoveInstance(std::string instanceId)
{
	int idx = GetIdx(instanceId);
	if (idx != -1) 
	{
		RemoveInstance(idx);
	}
}

unsigned int Model::GetIdx(std::string id)
{
	// test each instance
	for (int i = 0; i < currentNumInstances; i++) 
	{
		if (instances[i]->instanceId == id) 
		{
			return i;
		}
	}
	return -1;
}

void Model::ProcessNode(aiNode* node, const aiScene* scene)
{
	// process all meshes
	for (unsigned int i = 0; i < node->mNumMeshes; i++) 
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		Mesh newMesh = ProcessMesh(mesh, scene);
		AddMesh(&newMesh);
	}

	// process all child nodes
	for (unsigned int i = 0; i < node->mNumChildren; i++) 
	{
		ProcessNode(node->mChildren[i], scene);
	}
}

Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex>			vertices(mesh->mNumVertices);
	std::vector<unsigned int>	indices(3 * mesh->mNumFaces);
	std::vector<Texture>		textures;

	BoundingRegion br(BoundTypes::SPHERE);				// Setup bounding region
	glm::vec3 min(std::numeric_limits<float>::max());	// Min point = max float
	glm::vec3 max(std::numeric_limits<float>::min());	// Max point = min float

	//============================================================================================//
	//Process vectices
	//============================================================================================//

	for (int i = 0; i < mesh->mNumVertices; i++)
	{
		// Position
		vertices[i].pos = glm::vec3(
			mesh->mVertices[i].x, 
			mesh->mVertices[i].y, 
			mesh->mVertices[i].z
		);

		// determine if outside of current min and max
		for (int j = 0; j < 3; j++) 
		{
			// if smaller than min
			if (vertices[i].pos[j] < min[j]) min[j] = vertices[i].pos[j];
			// if larger than max
			if (vertices[i].pos[j] > max[j]) max[j] = vertices[i].pos[j];
		}

		// Normal vectors
		vertices[i].normal = glm::vec3(
			mesh->mNormals[i].x,
			mesh->mNormals[i].y,
			mesh->mNormals[i].z
		);

		// Textures
		if (mesh->mTextureCoords[0])
		{
			vertices[i].texCoord = glm::vec2(
				mesh->mTextureCoords[0][i].x,
				mesh->mTextureCoords[0][i].y
				);
		}
		else
		{
			vertices[i].texCoord = glm::vec2(0.0f);
		}

		// tangent vector
		if (mesh->mTangents)
		{
			vertices[i].tangent = 
			{
				mesh->mTangents[i].x,
				mesh->mTangents[i].y,
				mesh->mTangents[i].z
			};
		}	
		
	}

	//============================================================================================//
	//Process min/max for BR(bounding region)
	//============================================================================================//

	// Calculate max distance from the center
	br.center				= (min + max) / 2.0f;	
	br.ogCenter				= br.center;
	br.collisionMesh		= NULL;
	float maxRadiusSquared	= 0.0f;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++) 
	{
		float radiusSquared = 0.0f; // distance for this vertex
		for (int j = 0; j < 3; j++) 
		{
			radiusSquared += (vertices[i].pos[j] - br.center[j]) * (vertices[i].pos[j] - br.center[j]);
		}
		if (radiusSquared > maxRadiusSquared) 
		{
			// found new squared radius
			// a^2 > b^2 --> |a| > |b|
			maxRadiusSquared = radiusSquared;
		}
	}

	br.radius = sqrt(maxRadiusSquared);
	br.ogRadius = br.radius;

	//============================================================================================//
	//Process indices
	//============================================================================================//

	for (int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	// load data into mesh
	Mesh ret;

	//============================================================================================//
	//Process material
	//============================================================================================//

	if (mesh->mMaterialIndex >= 0) 
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		if (States::IsActive<unsigned int>(&switches, NO_TEX)) 
		{
			// use materials

			// 1. diffuse colors
			aiColor4D diff(1.0f);
			aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diff);
			// 2. specular colors
			aiColor4D spec(1.0f);
			aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &spec);

			ret = Mesh(br, diff, spec);
		}
		else 
		{
			// use textures

			// 1. diffuse maps
			std::vector<Texture> diffuseMaps = LoadTextures(material, aiTextureType_DIFFUSE, scene);
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
			// 2. specular maps
			std::vector<Texture> specularMaps = LoadTextures(material, aiTextureType_SPECULAR, scene);
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
			// 3. normal maps
			// .obj, use aiTextureType_HEIGHT
			std::vector<Texture> normalMaps = LoadTextures(material, aiTextureType_NORMALS, scene);
			textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

			//============================================================================================//
			//PBR Texture
			//============================================================================================//

			// albedoMap
			std::vector<Texture> pbrBaseColorMaps	= LoadTextures(material, aiTextureType_BASE_COLOR, scene);
			textures.insert(textures.end(), pbrBaseColorMaps.begin(), pbrBaseColorMaps.end());

			// normalMap
			std::vector<Texture> pbrNormalMaps		= LoadTextures(material, aiTextureType_NORMAL_CAMERA, scene);
			textures.insert(textures.end(), pbrNormalMaps.begin(), pbrNormalMaps.end());

			std::vector<Texture> pbrEmissionMaps	= LoadTextures(material, aiTextureType_EMISSION_COLOR, scene);
			textures.insert(textures.end(), pbrEmissionMaps.begin(), pbrEmissionMaps.end());

			// metallicMap
			std::vector<Texture> pbrMetalinessMaps	= LoadTextures(material, aiTextureType_METALNESS, scene);
			textures.insert(textures.end(), pbrMetalinessMaps.begin(), pbrMetalinessMaps.end());

			// roughnessMap
			std::vector<Texture> pbrRoughnessMaps	= LoadTextures(material, aiTextureType_DIFFUSE_ROUGHNESS, scene);
			textures.insert(textures.end(), pbrRoughnessMaps.begin(), pbrRoughnessMaps.end());

			// aoMap
			std::vector<Texture> pbrAOMaps		= LoadTextures(material, aiTextureType_AMBIENT_OCCLUSION, scene);
			textures.insert(textures.end(), pbrAOMaps.begin(), pbrAOMaps.end());

			ret = Mesh(br, textures);
		}
	}

	// load vertex and index data
	ret.LoadData(vertices, indices);
	return ret;
}

Mesh Model::ProcessMesh(BoundingRegion br, 
	unsigned int numVertices, float* vertices, 
	unsigned int numIndices, unsigned int* indices, 
	bool calcTanVectors, 
	unsigned int numCollisionPoints, float* collisionPoints, 
	unsigned int numCollisionFaces, unsigned int* collisionIndices, 
	bool pad)
{
	// Process vertex array
	std::vector<Vertex> vertexList = Vertex::GenList(vertices, numVertices);

	// Create index list
	std::vector<unsigned int> indexList(numIndices);

	if (indices) 
	{
		// Copy array
		memcpy(indexList.data(), indices, numIndices * sizeof(unsigned int));
	}
	else 
	{
		// Insert sequential indices
		for (unsigned int i = 0; i < numIndices; i++) 
		{
			indexList[i] = i;
		}
	}

	// Calculate the lighting values
	if (calcTanVectors) 
	{
		Vertex::CalcTanVectors(vertexList, indexList);
	}

	// Setup return mesh
	Mesh ret(br);
	ret.LoadData(vertexList, indexList, pad);

	// allocate collision mesh if specified
	if (numCollisionPoints) 
	{
		EnableCollisionModel();
		ret.LoadCollisionMesh(numCollisionPoints, collisionPoints, numCollisionFaces, collisionIndices);
	}

	return ret;
}

std::vector<Texture> Model::LoadTextures(aiMaterial* mat, aiTextureType type, const aiScene* scene)
{
	std::vector<Texture> textures;

	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		std::cout << str.C_Str() << std::endl;

		// Prevent duplicate load
		bool skip = false;
		for (unsigned int j = 0; j < texsLoaded.size(); j++)
		{
			if (std::strcmp(texsLoaded[j].path.data(), str.C_Str()) == 0)
			{
				textures.push_back(texsLoaded[j]);
				skip = true;
				break;
			}
		}

		if (!skip)
		{
			// Not loaded yet
			Texture tex(directory, str.C_Str(), type);
			
			// Does .fbx have embedded texture
			auto aitexture = scene->GetEmbeddedTexture(str.C_Str());
			if (aitexture != nullptr)
			{
				tex.LoadFromAssimp(aitexture);
			}
			else
			{
				tex.Load(false);
			}

			//tex.Load(false);
			textures.push_back(tex);
			texsLoaded.push_back(tex);
		}
	}

	return textures;
}
