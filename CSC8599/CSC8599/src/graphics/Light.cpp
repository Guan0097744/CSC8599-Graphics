#include "Light.h"

DirLight::DirLight() {}

DirLight::DirLight(glm::vec3 direction, glm::vec4 ambient, glm::vec4 diffuse, glm::vec4 specular, BoundingRegion br)
	: direction(direction),
	ambient(ambient), diffuse(diffuse), specular(specular),
	shadowFBO(2048, 2048, GL_DEPTH_BUFFER_BIT), br(br) 
{
	// setup FBO
	shadowFBO.Generate();

	shadowFBO.Bind();
	shadowFBO.DisableColorBuffer();
	shadowFBO.AllocateAndAttachTexture(GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT, GL_FLOAT);

	UpdateMatrices();
}

void DirLight::Render(Shader& shader, unsigned int textureIdx)
{
	// set depth texture
	glActiveTexture(GL_TEXTURE0 + textureIdx);
	shadowFBO.textures[0].Bind();
	shader.SetInt("dirLightBuffer", textureIdx);
}

void DirLight::UpdateMatrices()
{
	glm::mat4 proj			= glm::ortho(br.min.x, br.max.x, br.min.y, br.max.y, br.min.z, br.max.z);
	glm::vec3 pos			= -2.0f * direction;
	glm::mat4 lightView		= glm::lookAt(pos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	lightSpaceMatrix		= proj * lightView;
}

// list of directions
glm::vec3 PointLight::directions[6] = 
{
	{  1.0f,  0.0f,  0.0f },
	{ -1.0f,  0.0f,  0.0f },
	{  0.0f,  1.0f,  0.0f },
	{  0.0f, -1.0f,  0.0f },
	{  0.0f,  0.0f,  1.0f },
	{  0.0f,  0.0f, -1.0f }
};

// list of up vectors
glm::vec3 PointLight::ups[6] = 
{
	{  0.0f, -1.0f,  0.0f },
	{  0.0f, -1.0f,  0.0f },
	{  0.0f,  0.0f,  1.0f },
	{  0.0f,  0.0f, -1.0f },
	{  0.0f, -1.0f,  0.0f },
	{  0.0f, -1.0f,  0.0f }
};

PointLight::PointLight() {}

PointLight::PointLight(glm::vec3 position, float k0, float k1, float k2, glm::vec4 ambient, glm::vec4 diffuse, glm::vec4 specular, float nearPlane, float farPlane)
	: position(position),
	k0(k0), k1(k1), k2(k2),
	ambient(ambient), diffuse(diffuse), specular(specular),
	nearPlane(nearPlane), farPlane(farPlane),
	shadowFBO(2048, 2048, GL_DEPTH_BUFFER_BIT)
{
	shadowFBO.Generate();

	shadowFBO.Bind();
	shadowFBO.DisableColorBuffer();
	shadowFBO.AllocateAndAttachTexture(GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT, GL_FLOAT);

	UpdateMatrices();
}

void PointLight::Render(Shader& shader, int index, unsigned int textureIdx)
{
	// set depth texture
	glActiveTexture(GL_TEXTURE0 + textureIdx);
	shadowFBO.cubemap.Bind();
	shader.SetInt("pointLightBuffers[" + std::to_string(index) + "]", textureIdx);
}

void PointLight::UpdateMatrices()
{
	glm::mat4 proj = glm::perspective(glm::radians(90.0f),	// FOV
		(float)shadowFBO.height / (float)shadowFBO.width,	// aspect ratio
		nearPlane, farPlane);								// near and far bounds

	for (unsigned int i = 0; i < 6; i++) 
	{
		lightSpaceMatrices[i] = proj * glm::lookAt(position, position + PointLight::directions[i], PointLight::ups[i]);
	}
}

SpotLight::SpotLight() {}

SpotLight::SpotLight(glm::vec3 position, glm::vec3 direction, glm::vec3 up, float cutOff, float outerCutOff, float k0, float k1, float k2, glm::vec4 ambient, glm::vec4 diffuse, glm::vec4 specular, float nearPlane, float farPlane)
	: position(position), direction(direction), up(up),
	cutOff(cutOff), outerCutOff(outerCutOff),
	k0(k0), k1(k1), k2(k2),
	ambient(ambient), diffuse(diffuse), specular(specular),
	nearPlane(nearPlane), farPlane(farPlane),
	shadowFBO(2048, 2048, GL_DEPTH_BUFFER_BIT)
{
	shadowFBO.Generate();

	shadowFBO.Bind();
	shadowFBO.DisableColorBuffer();
	shadowFBO.AllocateAndAttachTexture(GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT, GL_FLOAT);

	UpdateMatrices();
}

void SpotLight::Render(Shader& shader, int index, unsigned int textureIdx)
{
	// set depth texture
	glActiveTexture(GL_TEXTURE0 + textureIdx);
	shadowFBO.textures[0].Bind();
	shader.SetInt("spotLightBuffers[" + std::to_string(index) + "]", textureIdx);
}

void SpotLight::UpdateMatrices()
{
	glm::mat4 proj = glm::perspective(glm::acos(outerCutOff) * 2.0f,	// FOV
		(float)shadowFBO.height / (float)shadowFBO.width,				// aspect ratio
		nearPlane, farPlane);											// near and far bounds

	glm::mat4 lightView		= glm::lookAt(position, position + direction, up);
	lightSpaceMatrix		= proj * lightView;
}

//PBRLight::PBRLight(glm::vec3 pos, glm::vec3 col)
//	: position(pos), color(col)
//{
//
//}
//
//void PBRLight::Render(Shader& shader, int index)
//{
//
//}
