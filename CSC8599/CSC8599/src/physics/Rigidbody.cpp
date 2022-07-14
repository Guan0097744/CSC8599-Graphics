#include "Rigidbody.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

bool RigidBody::operator==(RigidBody rb)
{
	return instanceId == rb.instanceId;
}

bool RigidBody::operator==(std::string id)
{
	return instanceId == id;
}

RigidBody::RigidBody(std::string modelId, glm::vec3 size, float mass, glm::vec3 pos, glm::vec3 rot)
	: modelId(modelId), size(size), mass(mass), pos(pos), rot(rot),
	velocity(0.0f), acceleration(0.0f), state(0),
	lastCollision(COLLISION_THRESHOLD), lastCollisionID("") 
{
	Update(0.0f);
}

/**
 * @brief Update position with velocity and acceleration
 * @param dt 
*/
void RigidBody::Update(float dt)
{
	pos			+= velocity * dt + 0.5f * acceleration * (dt * dt);
	velocity	+= acceleration * dt;

	// Calculate rotation matrix
	glm::mat4 rotMat = glm::toMat4(glm::quat(rot));

	//============================================================================================//
	//Model = trans * rot * scale = T * R * S
	//============================================================================================//
	
	model		= glm::translate(glm::mat4(1.0f), pos);	// M = I * T = T
	model		= model * rotMat;						// M = M * R = T * R
	model		= glm::scale(model, size);				// M = M * S = T * R * S

	normalModel = glm::transpose(glm::inverse(glm::mat3(model)));

	lastCollision += dt;
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

void RigidBody::HandleCollision(RigidBody* inst, glm::vec3 norm)
{
	if (lastCollision >= COLLISION_THRESHOLD || lastCollisionID != inst->instanceId) {
		this->velocity = glm::reflect(this->velocity, glm::normalize(norm));	// Register (elastic) collision
		lastCollision = 0.0f;	// Reset counter
	}

	lastCollisionID = inst->instanceId;
}
