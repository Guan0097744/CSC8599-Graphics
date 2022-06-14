#ifndef SPHERE_HPP
#define SPHERE_HPP

#include "../Model.h"

class Sphere :public Model
{
public:
	Sphere(glm::vec3 pos = glm::vec3(0.0f), glm::vec3 size = glm::vec3(1.0f)) :
		Model(pos, size, true) {}

	void Init()
	{
		LoadModel("assets/models/sphere/scene.gltf");
	}
};

#endif // !SPHERE_HPP
