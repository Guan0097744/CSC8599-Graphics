#ifndef SPHERE_HPP
#define SPHERE_HPP

#include "../Model.h"
#include "ModelArray.hpp"

class Sphere :public Model
{
public:
	Sphere(glm::vec3 pos = glm::vec3(0.0f), glm::vec3 size = glm::vec3(1.0f)) :
		Model(pos, size, true) 
	{
		rb.mass = 1.0f;
	}

	void Init()
	{
		LoadModel("assets/models/sphere/scene.gltf");
	}
};

class SphereArray : public ModelArray<Sphere>
{
public:
	void Init()
	{
		model = Sphere(glm::vec3(0.0f), glm::vec3(0.25f));
		ModelArray::Init();
	}
};

#endif // !SPHERE_HPP
