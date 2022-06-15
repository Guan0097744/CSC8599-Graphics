#include "Model.h"

#include "../physics/Environment.h"

Model::Model(glm::vec3 pos, glm::vec3 size, bool noTex) :
	/*pos(pos), */size(size), noTex(noTex)
{
	rb.pos = pos;
	//rb.acceleration = Environment::gravity;
}

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

void Model::Render(Shader& shader, float dt, bool setModel)
{
	rb.Update(dt);

	if (setModel)
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, rb.pos);
		model = glm::scale(model, size);
		shader.SetMat4("model", model);
	}

	shader.SetFloat("material.shininess", 0.5f);

	for (Mesh mesh : meshes)
	{
		mesh.Render(shader);
	}
}

void Model::Cleanup()
{
	for (Mesh mesh : meshes)
	{
		mesh.Cleanup();
	}
}

void Model::ProcessNode(aiNode* node, const aiScene* scene)
{
	// Process all meshes
	for (int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(ProcessMesh(mesh, scene));
	}

	// Process all child nodes
	for (int i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene);
	}
}

Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	// Process vectices
	for (int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;

		// Position
		vertex.pos = glm::vec3(
			mesh->mVertices[i].x, 
			mesh->mVertices[i].y, 
			mesh->mVertices[i].z
		);

		// Normal vectors
		vertex.normal = glm::vec3(
			mesh->mNormals[i].x,
			mesh->mNormals[i].y,
			mesh->mNormals[i].z
		);

		// Textures
		if (mesh->mTextureCoords[0])
		{
			vertex.texCoord = glm::vec2(
				mesh->mTextureCoords[0][i].x,
				mesh->mTextureCoords[0][i].y
				);
		}
		else
		{
			vertex.texCoord = glm::vec2(0.0f);
		}

		vertices.push_back(vertex);
	}

	// Process indices
	for (int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	// Process material
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

	if (noTex)
	{
		// Diffuse color
		aiColor4D diff(1.0f);
		aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diff);

		// Specular color
		aiColor4D spec(1.0f);
		aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &spec);

		return Mesh(vertices, indices, diff, spec);
	}

	// Diffuse maps
	std::vector<Texture> diffuseMaps = LoadTextures(material, aiTextureType_DIFFUSE, scene);
	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

	// Specular maps
	std::vector<Texture> specularMaps = LoadTextures(material, aiTextureType_SPECULAR, scene);
	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

	return Mesh(vertices, indices, textures);
}

//std::vector<Texture> Model::LoadTextures(aiMaterial* mat, aiTextureType type)
//{
//	std::vector<Texture> textures;
//
//	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
//	{
//		aiString str;
//		mat->GetTexture(type, i, &str);
//		std::cout << str.C_Str() << std::endl;
//
//		// Prevent duplicate load
//		bool skip = false;
//		for (unsigned int j = 0; j < texsLoaded.size(); j++)
//		{
//			if (std::strcmp(texsLoaded[j].GetPath().data(), str.C_Str()) == 0)
//			{
//				textures.push_back(texsLoaded[j]);
//				skip = true;
//				break;
//			}
//		}
//
//		if (!skip)
//		{
//			// Not loaded yet
//			Texture tex(directory, str.C_Str(), type);
//			tex.Load(false);
//			textures.push_back(tex);
//			texsLoaded.push_back(tex);
//		}
//	}
//
//	return textures;
//}

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
			if (std::strcmp(texsLoaded[j].GetPath().data(), str.C_Str()) == 0)
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
