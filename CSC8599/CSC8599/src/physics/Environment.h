#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include <glm/glm.hpp>

class Environment
{
public:
	static glm::vec3 worldUp;	// Up vector in world
	static glm::vec3 gravity;	// Acceleration of gravity
};

#endif // !ENVIRONMENT_H

