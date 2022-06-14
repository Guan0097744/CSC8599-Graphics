#include "Rigidbody.h"

RigidBody::RigidBody(float mass, glm::vec3 pos, glm::vec3 velocity, glm::vec3 acceleration) :
	mass(mass), pos(pos), velocity(velocity), acceleration(acceleration) 
{

}

/**
 * @brief Update position with velocity and acceleration
 * @param dt 
*/
void RigidBody::Update(float dt)
{
	pos += velocity * dt + 0.5f * acceleration * (dt * dt);
	velocity += acceleration * dt;
}
