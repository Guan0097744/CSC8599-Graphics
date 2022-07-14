#ifndef PLANE_HPP
#define PLANE_HPP

#include "../Model.h"

class Plane : public Model 
{
public:
    Plane()
        : Model("plane", 1, CONST_INSTANCES) {}

    void Init(std::vector<Texture> textures) 
    {
        int numVertices = 4;

        float quadVertices[] = 
        {
            // position			normal				texcoord
             0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f,	1.0f, 1.0f, // top right
            -0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f,	0.0f, 1.0f, // top left
            -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,	0.0f, 0.0f, // bottom left
             0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,	1.0f, 0.0f  // bottom right
        };
        float collisionVertices[] = 
        {
            // position			
             0.5f,  0.5f, 0.0f,
            -0.5f,  0.5f, 0.0f,
            -0.5f, -0.5f, 0.0f,
             0.5f, -0.5f, 0.0f
        };

        unsigned int indices[] = 
        {
            0, 1, 3,
            1, 2, 3
        };

        //BoundingRegion br(glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0.5f, 0.5f, 0.0f));
        BoundingRegion br(glm::vec3(0.0f), 1 / sqrt(2.0f));

        Mesh ret = ProcessMesh(br,
            numVertices, quadVertices,
            6, indices,
            true,
            numVertices, collisionVertices,
            2, indices);

        ret.SetupTextures(textures);

        AddMesh(&ret);
    }
};

#endif