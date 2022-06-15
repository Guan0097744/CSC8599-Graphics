#ifndef MODELARRAY_HPP
#define MODELARRAY_HPP

#include "../Model.h"

#define UPPER_BOUND 100

template<class T>
class ModelArray
{
public:
	std::vector<RigidBody> instances;

	void Init()
	{
		model.Init();

		//============================================================================================//
		//Generate position VBO
		//============================================================================================//

		glGenBuffers(1, &posVBO);
		glBindBuffer(GL_ARRAY_BUFFER, posVBO);
		glBufferData(GL_ARRAY_BUFFER, UPPER_BOUND * 3 * sizeof(float), NULL, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//============================================================================================//
		//Generate size VBO
		//============================================================================================//

		glGenBuffers(1, &sizeVBO);
		glBindBuffer(GL_ARRAY_BUFFER, sizeVBO);
		glBufferData(GL_ARRAY_BUFFER, UPPER_BOUND * 3 * sizeof(float), NULL, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//============================================================================================//
		//Set atttribute pointers for each mesh
		//============================================================================================//

		for (int i = 0; i < model.meshes.size(); i++)
		{
			glBindVertexArray(model.meshes[i].VAO);

			//Model Position
			glBindBuffer(GL_ARRAY_BUFFER, posVBO);
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
			glEnableVertexAttribArray(3);
			//Model sizes
			glBindBuffer(GL_ARRAY_BUFFER, sizeVBO);
			glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
			glEnableVertexAttribArray(4);

			glBindBuffer(GL_ARRAY_BUFFER, 0);

			//Reset 3rd attribute every 1 instance
			glVertexAttribDivisor(3, 1);
			////Reset 4th attribute every 1 instance
			glVertexAttribDivisor(4, 1);

			glBindVertexArray(0);
		}

	}

	void Render(Shader shader, float dt)
	{
		for (auto& rb : instances)
		{
			rb.Update(dt);
			model.rb.pos = rb.pos;
			model.Render(shader, dt);
		}
	}

	void SetSize(glm::vec3 size)
	{
		model.size = size;
	}

	void Cleanup()
	{
		model.Cleanup();
	}

protected:
	T model;

	unsigned int posVBO;
	unsigned int sizeVBO;
	std::vector<glm::vec3> position;
	std::vector<glm::vec3> sizes;
};

#endif // !MODELARRAY_HPP

