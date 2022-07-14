#ifndef SPHERE_HPP
#define SPHERE_HPP

#include "../Model.h"

class Sphere :public Model
{
public:
	Sphere(unsigned int maxNumInstances = 1)
		: Model("sphere", maxNumInstances, NO_TEX | DYNAMIC) {}

	void Init() 
	{
		LoadModel("assets/models/sphere/scene.gltf");
	}
};

#endif // !SPHERE_HPP
