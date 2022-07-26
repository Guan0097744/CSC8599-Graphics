#include "Mesh.h"

std::vector<Vertex> Vertex::GenList(float* vertices, int numVertices)
{
	std::vector<Vertex> ret(numVertices);

	//int stride = sizeof(Vertex) / sizeof(float);
	int stride = 8;

	for (int i = 0; i < numVertices; i++)
	{
		ret[i].pos = glm::vec3(
			vertices[i * stride + 0],
			vertices[i * stride + 1],
			vertices[i * stride + 2]
		);

		ret[i].normal = glm::vec3(
			vertices[i * stride + 3],
			vertices[i * stride + 4],
			vertices[i * stride + 5]
		);

		ret[i].texCoord = glm::vec2(
			vertices[i * stride + 6],
			vertices[i * stride + 7]
		);
	}

	return ret;
}

void Vertex::AverageVectors(glm::vec3& baseVec, glm::vec3 addition, unsigned char existingContributions)
{
	if (!existingContributions)
	{
		baseVec = addition;
	}
	else
	{
		float f = 1 / ((float)existingContributions + 1);

		baseVec *= (float)(existingContributions)*f;

		baseVec += addition * f;
	}
}

/**
 * @brief Calculate tangent vectors for each face
 * @param list 
 * @param indices 
*/
void Vertex::CalcTanVectors(std::vector<Vertex>& list, std::vector<unsigned int>& indices) 
{
	unsigned char* counts = (unsigned char*)malloc(list.size() * sizeof(unsigned char));
	for (unsigned int i = 0, len = list.size(); i < len; i++) 
	{
		counts[i] = 0;
	}

	// Iterate through indices and calculate vectors for each face
	for (unsigned int i = 0, len = indices.size(); i < len; i += 3) 
	{
		// 3 vertices corresponding to the face
		Vertex v1 = list[indices[i + 0]];
		Vertex v2 = list[indices[i + 1]];
		Vertex v3 = list[indices[i + 2]];

		// Calculate edges
		glm::vec3 edge1 = v2.pos - v1.pos;
		glm::vec3 edge2 = v3.pos - v1.pos;

		// Calculate delta UVs
		glm::vec2 deltaUV1 = v2.texCoord - v1.texCoord;
		glm::vec2 deltaUV2 = v3.texCoord - v1.texCoord;

		// use inverse of the UV matrix to determine tangent
		float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);

		glm::vec3 tangent = {
			f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x),
			f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y),
			f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z)
		};

		// average in the new tangent vector
		AverageVectors(list[indices[i + 0]].tangent, tangent, counts[indices[i + 0]]++);
		AverageVectors(list[indices[i + 1]].tangent, tangent, counts[indices[i + 1]]++);
		AverageVectors(list[indices[i + 2]].tangent, tangent, counts[indices[i + 2]]++);
	}
}

Mesh::Mesh()
	: collision(NULL) {}

/**
 * @brief Intialize with a bounding region
 * @param br 
*/
Mesh::Mesh(BoundingRegion br)
	: br(br), collision(NULL) {}

/**
 * @brief Initialize as textured object
 * @param br 
 * @param textures 
*/
Mesh::Mesh(BoundingRegion br, std::vector<Texture> textures)
	: Mesh(br) 
{
	SetupTextures(textures);
}

/**
 * @brief Initialize as material object
 * @param br 
 * @param diff 
 * @param spec 
*/
Mesh::Mesh(BoundingRegion br, aiColor4D diff, aiColor4D spec)
	: Mesh(br) 
{
	SetupColors(diff, spec);
}

/**
 * @brief initialize with a material
 * @param br 
 * @param m 
*/
Mesh::Mesh(BoundingRegion br, Material m)
	: Mesh(br) 
{
	SetupMaterial(m);
}

void Mesh::LoadData(std::vector<Vertex> vertices, std::vector<unsigned int> indices, bool pad)
{
	this->vertices	= vertices;
	this->indices	= indices;

	// Bind VAO
	VAO.Generate();
	VAO.Bind();

	// Generate/set EBO
	VAO["EBO"] = BufferObject(GL_ELEMENT_ARRAY_BUFFER);
	VAO["EBO"].Generate();
	VAO["EBO"].Bind();
	VAO["EBO"].SetData<GLuint>(this->indices.size(), &this->indices[0], GL_STATIC_DRAW);

	// Load data into vertex buffers
	VAO["VBO"] = BufferObject(GL_ARRAY_BUFFER);
	VAO["VBO"].Generate();
	VAO["VBO"].Bind();

	unsigned int size = this->vertices.size();
	if (pad && size) 
	{
		size++;
	}

	VAO["VBO"].SetData<Vertex>(size, &this->vertices[0], GL_STATIC_DRAW);

	// Set the vertex attribute pointers
	VAO["VBO"].Bind();
	// Vertex Positions
	VAO["VBO"].SetAttPointer<GLfloat>(0, 3, GL_FLOAT, 11, 0);
	// Normal ray
	VAO["VBO"].SetAttPointer<GLfloat>(1, 3, GL_FLOAT, 11, 3);
	// Vertex texture coords
	VAO["VBO"].SetAttPointer<GLfloat>(2, 2, GL_FLOAT, 11, 6);
	// Tangent vector
	VAO["VBO"].SetAttPointer<GLfloat>(3, 3, GL_FLOAT, 11, 8);

	VAO["VBO"].Clear();

	ArrayObject::Clear();
}

void Mesh::LoadCollisionMesh(unsigned int noPoints, float* coordinates, unsigned int noFaces, unsigned int* indices)
{
	this->collision		= new CollisionMesh(noPoints, coordinates, noFaces, indices);
	this->br			= this->collision->br;
}

void Mesh::SetupTextures(std::vector<Texture> textures)
{
	this->noTex			= false;
	this->textures.insert(this->textures.end(), textures.begin(), textures.end());
}

void Mesh::SetupColors(aiColor4D diff, aiColor4D spec)
{
	this->noTex			= true;
	this->diffuse		= diff;
	this->specular		= spec;
}

void Mesh::SetupMaterial(Material mat)
{
	this->noTex			= true;
	this->diffuse		= { mat.diffuse.r, mat.diffuse.g, mat.diffuse.b, 1.0f };
	this->specular		= { mat.specular.r, mat.specular.g, mat.specular.b, 1.0f };
}

void Mesh::Render(Shader& shader, unsigned int numInstances)
{
	shader.SetBool("noNormalMap", true);

	if (noTex)
	{
		// Set materials
		shader.Set4Float("material.diffuse", diffuse);
		shader.Set4Float("material.specular", specular);
		shader.SetInt("noTex", 1);
	}
	else
	{
		// Set Textures
		unsigned int diffuseIdx		= 0;
		unsigned int normalIdx		= 0;
		unsigned int specularIdx	= 0;

		for (int i = 0; i < textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);

			std::string name;
			switch (textures[i].type)
			{
			case aiTextureType_DIFFUSE:
				name = "diffuse" + std::to_string(diffuseIdx++);
				break;
			case aiTextureType_NORMALS:
				name = "normal" + std::to_string(normalIdx++);
				shader.SetBool("noNormalMap", false);
				break;
			case aiTextureType_SPECULAR:
				name = "specular" + std::to_string(specularIdx++);
				break;
			case aiTextureType_DIFFUSE_ROUGHNESS:
				break;
			case aiTextureType_METALNESS:
				break;
			default:
				name = textures[i].name;
				break;
			}

			shader.SetInt(name, i);
			textures[i].Bind();
		}

		shader.SetBool("noTex", false);
	}

	VAO.Bind();
	VAO.Draw(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, numInstances);
	ArrayObject::Clear();

	glActiveTexture(GL_TEXTURE0);
}

void Mesh::Cleanup()
{
	VAO.Cleanup();

	for (Texture t : textures) 
	{
		t.Cleanup();
	}
}

void Mesh::Setup()
{
	// create buffers/arrays

	// bind VAO
	VAO.Generate();
	VAO.Bind();

	// generate/set EBO
	VAO["EBO"] = BufferObject(GL_ELEMENT_ARRAY_BUFFER);
	VAO["EBO"].Generate();
	VAO["EBO"].Bind();
	VAO["EBO"].SetData<GLuint>(indices.size(), &indices[0], GL_STATIC_DRAW);

	// generate/set VBO
	VAO["VBO"] = BufferObject(GL_ARRAY_BUFFER);
	VAO["VBO"].Generate();
	VAO["VBO"].Bind();
	VAO["VBO"].SetData<Vertex>(vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// set vertex attrib pointers
	// vertex positions
	VAO["VBO"].SetAttPointer<GLfloat>(0, 3, GL_FLOAT, 8, 0);
	// normal ray
	VAO["VBO"].SetAttPointer<GLfloat>(1, 3, GL_FLOAT, 8, 3);
	// texture coordinates
	VAO["VBO"].SetAttPointer<GLfloat>(2, 2, GL_FLOAT, 8, 6);

	VAO["VBO"].Clear();

	ArrayObject::Clear();
}
