#ifndef RAY_H
#define RAY_H

#include <glm/glm.hpp>

#include "Bounds.h"
#include "../physics/Collisionmesh.h"
#include "../physics/Rigidbody.h"

class Ray 
{
public:
	glm::vec3 origin;
	glm::vec3 dir;
	glm::vec3 invdir;

	Ray(glm::vec3 origin, glm::vec3 dir);

	bool IntersectsBoundingRegion(BoundingRegion br, float& tmin, float& tmax);
	bool IntersectsMesh(CollisionMesh* mesh, RigidBody* rb, float& t);
};

#endif