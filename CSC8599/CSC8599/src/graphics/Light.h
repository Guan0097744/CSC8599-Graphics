#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>

#include "Shader.h"
#include "../algorithms/Bounds.h"
#include "buffer/FrameBuffer.hpp"

struct DirLight
{
	glm::vec3			direction;

	glm::vec4			ambient;
	glm::vec4			diffuse;
	glm::vec4			specular;

	BoundingRegion		br;						// bounding region for shadow
	glm::mat4			lightSpaceMatrix;		// transformation to light space (projection * view)
	FramebufferObject	shadowFBO;				// FBO for shadows

	DirLight();
	DirLight(glm::vec3 direction,
		glm::vec4 ambient,
		glm::vec4 diffuse,
		glm::vec4 specular,
		BoundingRegion br);

	void Render(Shader& shader, unsigned int textureIdx);
	void UpdateMatrices();						// update light space matrix
};

struct PointLight
{
	glm::vec3			position;

	// Attenuation constants
	float				k0;						// constant;
	float				k1;						// linear;
	float				k2;						// quadratic;

	glm::vec4			ambient;
	glm::vec4			diffuse;
	glm::vec4			specular;

	float				nearPlane;
	float				farPlane;

	glm::mat4			lightSpaceMatrices[6];	// list of view matrices
	FramebufferObject	shadowFBO;				// FBO for shadows

	static glm::vec3	directions[6];			// list of directions
	static glm::vec3	ups[6];					// list of up vectors

	PointLight();
	PointLight(glm::vec3 position,
		float k0, float k1, float k2,
		glm::vec4 ambient, glm::vec4 diffuse, glm::vec4 specular,
		float nearPlane, float farPlane);

	void Render(Shader& shader, int index, unsigned int textureIdx);
	void UpdateMatrices();
};

struct SpotLight
{
	glm::vec3			position;
	glm::vec3			direction;
	glm::vec3			up;

	float				cutOff;
	float				outerCutOff;

	// Attenuation constants
	float				k0;						// constant;
	float				k1;						// linear;
	float				k2;						// quadratic;

	glm::vec4			ambient;
	glm::vec4			diffuse;
	glm::vec4			specular;

	float				nearPlane;
	float				farPlane;

	glm::mat4			lightSpaceMatrix;		// transformation to light space (projection * view)
	FramebufferObject	shadowFBO;				// FBO for shadows

	SpotLight();
	SpotLight(glm::vec3 position, glm::vec3 direction, glm::vec3 up,
		float cutOff, float outerCutOff,
		float k0, float k1, float k2,
		glm::vec4 ambient, glm::vec4 diffuse, glm::vec4 specular,
		float nearPlane, float farPlane);

	void Render(Shader& shader, int index, unsigned int textureIdx);
	void UpdateMatrices();

};

class Light
{
public:
	glm::vec4			ambient;
	glm::vec4			diffuse;
	glm::vec4			specular;

	Light();

	virtual void Render();
	virtual void UpdateMatrices();
};

#endif

