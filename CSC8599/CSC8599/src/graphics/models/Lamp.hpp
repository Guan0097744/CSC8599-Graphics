#ifndef LAMP_HPP
#define LAMP_HPP

#include "Cube.hpp"
#include "../Light.h"
#include "ModelArray.hpp"

class Lamp :public Cube
{
public:
	glm::vec3 lightColor;
	
	PointLight pointLight;

	Lamp() {}

	/**
	 * @brief Lamp
	 * @param lightColor 
	 * @param ambient 
	 * @param diffuse 
	 * @param specular 
	 * @param constant 
	 * @param linear 
	 * @param quadratic 
	 * @param pos 
	 * @param size 
	*/
	Lamp(glm::vec3 lightColor,
		glm::vec4 ambient,
		glm::vec4 diffuse,
		glm::vec4 specular,
		float constant,
		float linear,
		float quadratic,
		glm::vec3 pos,
		glm::vec3 size) :
		lightColor(lightColor),
		pointLight({ pos, constant, linear, quadratic, ambient, diffuse, specular }),
		Cube(pos, size) {}

	void Render(Shader shader, float dt)
	{
		shader.Set3Float("lightColor", lightColor);

		Cube::Render(shader, dt);
	}
};

class LampArray :public ModelArray<Lamp>
{
public:
	std::vector<PointLight> instances;

	void Init()
	{
		model = Lamp(glm::vec3(1.0f),
			glm::vec4(0.05f, 0.05f, 0.05f, 1.0f), glm::vec4(0.8f, 0.8f, 0.8f, 1.0f), glm::vec4(1.0f),
			1.0f, 0.07f, 0.03f,
			glm::vec3(0.0f), glm::vec3(0.25f));

		ModelArray::Init();
	}

	void Render(Shader shader, float dt)
	{
		for (auto light : instances)
		{
			model.rb.pos = light.position;

			model.Render(shader, dt);
		}
	}
};

#endif // !LAMP_HPP
