#include "Light.h"

void DirLight::Render(Shader shader)
{
	std::string name = "dirLight";

	shader.Set3Float(name + ".direction", direction);
	shader.Set3Float(name + ".ambient", ambient);
	shader.Set3Float(name + ".diffuse", diffuse);
	shader.Set3Float(name + ".specular", specular);
}

void PointLight::Render(int index, Shader shader)
{
	std::string name = "pointLights[" + std::to_string(index) + "]";

	shader.Set3Float(name + ".position", position);

	shader.SetFloat(name + ".constant", constant);
	shader.SetFloat(name + ".linear", linear);
	shader.SetFloat(name + ".quadratic", quadratic);

	shader.Set3Float(name + ".ambient", ambient);
	shader.Set3Float(name + ".diffuse", diffuse);
	shader.Set3Float(name + ".specular", specular);
}

void SpotLight::Render(int index, Shader shader)
{
	std::string name = "spotLights[" + std::to_string(index) + "]";

	shader.Set3Float(name + ".position", position);
	shader.Set3Float(name + ".direction", direction);

	shader.SetFloat(name + ".cutOff", cutOff);
	shader.SetFloat(name + ".outerCutOff", outerCutOff);

	shader.SetFloat(name + ".constant", constant);
	shader.SetFloat(name + ".linear", linear);
	shader.SetFloat(name + ".quadratic", quadratic);

	shader.Set3Float(name + ".ambient", ambient);
	shader.Set3Float(name + ".diffuse", diffuse);
	shader.Set3Float(name + ".specular", specular);
}
