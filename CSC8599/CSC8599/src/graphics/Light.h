#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>

#include "Shader.h"

struct DirLight
{
	glm::vec3 direction;

	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;

	void Render(Shader shader);
};

struct PointLight
{
	glm::vec3 position;

	// Attenuation constants
	float constant;
	float linear;
	float quadratic;

	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;

	void Render(int index, Shader shader);
};

struct SpotLight
{
	glm::vec3 position;
	glm::vec3 direction;

	float cutOff;
	float outerCutOff;

	// Attenuation constants
	float constant;
	float linear;
	float quadratic;

	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;

	void Render(int index, Shader shader);
};

#endif

