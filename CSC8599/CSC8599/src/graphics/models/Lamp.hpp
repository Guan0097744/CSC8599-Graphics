#ifndef LAMP_HPP
#define LAMP_HPP

#include "Cube.hpp"

class Lamp :public Cube
{
public:
	glm::vec3 lightColor;
	
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	// lightColor, ambient, diffuse, specular, pos, size
	Lamp(glm::vec3 lightColor,
		glm::vec3 ambient,
		glm::vec3 diffuse,
		glm::vec3 specular,
		glm::vec3 pos,
		glm::vec3 size) :
		lightColor(lightColor),
		ambient(ambient),
		diffuse(diffuse),
		specular(specular),
		Cube(Material::white_plastic, pos, size)
	{
	}

	void Render(Shader shader)
	{
		shader.Set3Float("lightColor", lightColor);

		Cube::Render(shader);
	}
};

#endif // !LAMP_HPP
