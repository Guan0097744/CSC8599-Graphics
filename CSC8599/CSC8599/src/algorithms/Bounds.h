#ifndef BOUNDS_H
#define BOUNDS_H

#include <glm/glm.hpp>

#include "../physics/Rigidbody.h"

// Forward declaration
namespace Octree 
{
    class node;
}
class CollisionMesh;

// Possible Bound Types
enum class BoundTypes : unsigned char 
{
    AABB = 0x00,	// 0x00 = 0	// Axis-aligned bounding box
    SPHERE = 0x01	// 0x01 = 1
};

class BoundingRegion 
{
public:
    // Region type
    BoundTypes          type;

    // Quick access to instance and collision mesh
    RigidBody*          instance;
    CollisionMesh*      collisionMesh;

    // Quick access to current octree node
    Octree::node*       cell;

    //============================================================================================//
    //Sphere values
    //============================================================================================//

    glm::vec3           center;
    float               radius;

    glm::vec3           ogCenter;
    float               ogRadius;

    //============================================================================================//
    //Bounding box values
    //============================================================================================//
    
    glm::vec3           min;
    glm::vec3           max;

    glm::vec3           ogMin;
    glm::vec3           ogMax;


    // Initialize with type
    BoundingRegion(BoundTypes type = BoundTypes::AABB);

    // Initialize as sphere
    BoundingRegion(glm::vec3 center, float radius);

    // Initialize as AABB
    BoundingRegion(glm::vec3 min, glm::vec3 max);

    //============================================================================================//
    //Calculating values for the region
    //============================================================================================//
    
    // Transform for instance
    void Transform();

    // Center
    glm::vec3 CalculateCenter();

    // Calculate dimensions
    glm::vec3 CalculateDimensions();

    //============================================================================================//
    //Testing methods
    //============================================================================================//

    // Determine if point inside
    bool ContainsPoint(glm::vec3 pt);

    // Determine if region completely inside
    bool ContainsRegion(BoundingRegion br);

    // Determine if region intersects (partial containment)
    bool IntersectsWith(BoundingRegion br);

    // Operator overload
    bool operator==(BoundingRegion br);
};

#endif