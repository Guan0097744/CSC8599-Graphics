#include "Rigidbody.h"

RigidBody::RigidBody(float mass, glm::vec3 pos, glm::vec3 velocity, glm::vec3 acceleration) :
	mass(mass), pos(pos), velocity(velocity), acceleration(acceleration) {}

/**
 * @brief Update position with velocity and acceleration
 * @param dt 
*/
void RigidBody::Update(float dt)
{
	pos += velocity * dt + 0.5f * acceleration * (dt * dt);
	velocity += acceleration * dt;
}

void RigidBody::ApplyForce(glm::vec3 force)
{
	acceleration += force / mass;
}

void RigidBody::ApplyForce(glm::vec3 direction, float magnitude)
{
	ApplyForce(direction * magnitude);
}

void RigidBody::ApplyImpulse(glm::vec3 force, float dt)
{
	velocity += force / mass * dt;
}

void RigidBody::ApplyImpulse(glm::vec3 direction, float magnitude, float dt)
{
	ApplyImpulse(direction * magnitude, dt);
}

/**
 * @brief 
 * @param a = acceleration 
*/
void RigidBody::ApplyAcceleration(glm::vec3 a)
{
	acceleration += a;
}

void RigidBody::ApplyAcceleration(glm::vec3 direction, float magnitude)
{
	ApplyAcceleration(direction * magnitude);
}

void RigidBody::TransferEnergy(float joules, glm::vec3 direction)
{
	if (joules == 0)
	{
		return;
	}

	glm::vec3 delta = sqrt(2 * abs(joules) / mass) * direction;

	velocity += joules > 0 ? delta : -delta;
}
