#ifndef LAMP_HPP
#define LAMP_HPP

#include "Cube.hpp"
#include "../Light.h"

class Lamp :public Cube
{
public:
	glm::vec3 lightColor;
	
	PointLight pointLight;

	Lamp() {}

	// lightColor, ambient, diffuse, specular, pos, size
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

#endif // !LAMP_HPP
