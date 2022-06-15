#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include <glm/glm.hpp>

#include <string>

// switches for instance states
#define INSTANCE_DEAD		(unsigned char)0b00000001
#define INSTANCE_MOVED		(unsigned char)0b00000010

#define COLLISION_THRESHOLD 0.05f

//============================================================================================//
//Rigid Body class -represents physical body and holds all parameters
//============================================================================================//

class RigidBody {
public:
    // combination of switches above
    unsigned char state;

    // mass in kg
    float mass;
    // position in m
    glm::vec3 pos;
    // velocity in m/s
    glm::vec3 velocity;
    // acceleration in m/s^2
    glm::vec3 acceleration;

    RigidBody(float mass = 1.0f, glm::vec3 pos = glm::vec3(0.0f), glm::vec3 velocity = glm::vec3(0.0f), glm::vec3 acceleration = glm::vec3(0.0f));

    void Update(float dt);

    //============================================================================================//
    //Apply a force
    //============================================================================================//

    void ApplyForce(glm::vec3 force);
    void ApplyForce(glm::vec3 direction, float magnitude);

    //============================================================================================//
    //Apply force over time
    //============================================================================================//

    void ApplyImpulse(glm::vec3 force, float dt);
    void ApplyImpulse(glm::vec3 direction, float magnitude, float dt);

    //============================================================================================//
    //Apply an acceleration (remove redundancy of dividing by mass)
    //============================================================================================//

    void ApplyAcceleration(glm::vec3 a);
    void ApplyAcceleration(glm::vec3 direction, float magnitude);

    //============================================================================================//
    //Transfer potential or kinetic energy from another object
    //============================================================================================//

    void TransferEnergy(float joules, glm::vec3 direction);

    //============================================================================================//
    //Collisions
    //============================================================================================//

    void HandleCollision(RigidBody* inst, glm::vec3 norm);
};

#endif