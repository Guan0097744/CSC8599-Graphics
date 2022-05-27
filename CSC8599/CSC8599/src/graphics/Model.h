#ifndef MODEL_H
#define MODEL_H

#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Mesh.h"

class Model
{
public:
	std::vector<Mesh> meshes;
	
	Model();

	void Init();

	void Render(Shader shader);

	void Cleanup();
};


#endif // !MODEL_H
