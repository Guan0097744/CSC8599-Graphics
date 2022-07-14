#ifndef OCTREE_H
#define OCTREE_H

#define NO_CHILDREN 8
#define MIN_BOUNDS 0.5

#include <vector>
#include <queue>
#include <stack>

#include "../List.hpp"
#include "../States.hpp"
#include "../Bounds.h"
#include "../Ray.h"

#include "../../graphics/Model.h"

//============================================================================================//
//Forward declaration
//============================================================================================//

class Model;
class BoundingRegion;
class Box;

/**
 * @brief namespace to tie together all classes and functions relating to octree
*/
namespace Octree 
{
    // enum to represent octants
    enum class Octant : unsigned char 
    {
        O1 = 0x01,	// = 0b00000001
        O2 = 0x02,	// = 0b00000010
        O3 = 0x04,	// = 0b00000100
        O4 = 0x08,	// = 0b00001000
        O5 = 0x10,	// = 0b00010000
        O6 = 0x20,	// = 0b00100000
        O7 = 0x40,	// = 0b01000000
        O8 = 0x80	// = 0b10000000
    };

    //============================================================================================//
    //Utility methods callbacks
    //============================================================================================//

    // Calculate bounds of specified quadrant in bounding region
    void CalculateBounds(BoundingRegion& out, Octant octant, BoundingRegion parentRegion);

    /**
     * @brief Class to represent each node in the octree
    */
    class node 
    {
    public:
        node*           parent;                     // parent pointer
        node*           children[NO_CHILDREN];      // array of children (8)
        unsigned char   activeOctants;              // switch for active octants
        bool            treeReady = false;          // if tree is ready
        bool            treeBuilt = false;          // if tree is built
        short           maxLifespan = 8;            // maximum possible lifespan
        short           currentLifespan = -1;       // current lifespace

        std::vector<BoundingRegion> objects;        // list of objects in node
        std::queue<BoundingRegion>  queue;          // queue of objects to be dynamically inserted

        BoundingRegion  region;                     // region of bounds of cell (AABB)

        // default
        node();

        // initialize with bounds (no objects yet)
        node(BoundingRegion bounds);

        // initialize with bounds and list of objects
        node(BoundingRegion bounds, std::vector<BoundingRegion> objectList);

        // add instance to pending queue
        void AddToPending(RigidBody* instance, Model* model);

        // build tree (called during initialization)
        void Build();

        // update objects in tree (called during each iteration of main loop)
        void Update(Box& box);

        // process pending queue
        void ProcessPending();

        // dynamically insert object into node
        bool Insert(BoundingRegion obj);

        // check collisions with all objects in node
        void CheckCollisionsSelf(BoundingRegion obj);

        // check collisions with all objects in child nodes
        void CheckCollisionsChildren(BoundingRegion obj);

        // check collisions with a ray
        BoundingRegion* CheckCollisionsRay(Ray r, float& tmin);

        // destroy object (free memory)
        void Destroy();
    };
}

#endif