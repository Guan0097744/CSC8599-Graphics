#ifndef BOX_HPP
#define BOX_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <vector>

#include "../buffer/VertexBuffer.hpp"
#include "../../algorithms/Bounds.h"
#include "../Shader.h"

#define UPPER_BOUND 100

class Box 
{
public:
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> sizes;

    void Init() 
    {
        vertices = 
        {
            // position             x   y   z   i
             0.5f,  0.5f,  0.5f, // +   +   +   0
            -0.5f,  0.5f,  0.5f, // -   +   +   1
            -0.5f, -0.5f,  0.5f, // -   -   +   2
             0.5f, -0.5f,  0.5f, // +   -   +   3
             0.5f, 0.5f, -0.5f,  // +   +   -   4
            -0.5f, 0.5f, -0.5f,  // -   +   -   5
            -0.5f, -0.5f, -0.5f, // -   -   -   6
             0.5f, -0.5f, -0.5f  // +   -   -   7
        };

        indices = { // 12 lines
            // front face (+ve z)
            0, 1,
            1, 2,
            2, 3,
            3, 0,
            // back face (-ve z)
            4, 5,
            5, 6,
            6, 7,
            7, 4,
            // right face (+ve x)
            0, 4,
            3, 7,
            // left face (-ve x)
            1, 5,
            2, 6
        };

        // generate VAO
        VAO.Generate();
        VAO.Bind();

        // generate EBO
        VAO["EBO"] = BufferObject(GL_ELEMENT_ARRAY_BUFFER);
        VAO["EBO"].Generate();
        VAO["EBO"].Bind();
        VAO["EBO"].SetData<GLuint>(indices.size(), &indices[0], GL_STATIC_DRAW);

        // generate VBO
        VAO["VBO"] = BufferObject(GL_ARRAY_BUFFER);
        VAO["VBO"].Generate();
        VAO["VBO"].Bind();
        VAO["VBO"].SetData<GLfloat>(vertices.size(), &vertices[0], GL_STATIC_DRAW);
        VAO["VBO"].SetAttPointer<GLfloat>(0, 3, GL_FLOAT, 3, 0);

        // position VBO - dynamic
        VAO["posVBO"] = BufferObject(GL_ARRAY_BUFFER);
        VAO["posVBO"].Generate();
        VAO["posVBO"].Bind();
        VAO["posVBO"].SetData<glm::vec3>(UPPER_BOUND, NULL, GL_DYNAMIC_DRAW);
        VAO["posVBO"].SetAttPointer<glm::vec3>(1, 3, GL_FLOAT, 1, 0, 1);

        // size VBO - dynamic
        VAO["sizeVBO"] = BufferObject(GL_ARRAY_BUFFER);
        VAO["sizeVBO"].Generate();
        VAO["sizeVBO"].Bind();
        VAO["sizeVBO"].SetData<glm::vec3>(UPPER_BOUND, NULL, GL_DYNAMIC_DRAW);
        VAO["sizeVBO"].SetAttPointer<glm::vec3>(2, 3, GL_FLOAT, 1, 0, 1);
        VAO["sizeVBO"].Clear();

        ArrayObject::Clear();
    }

    void Render(Shader shader) 
    {
        shader.SetMat4("model", glm::mat4(1.0f));

        // update data
        int instances = std::min(UPPER_BOUND, (int)positions.size()); // if more than 100 instances, only render 100

        // update data
        if (instances != 0) 
        {
            // if instances exist

            // update data
            VAO["posVBO"].Bind();
            VAO["posVBO"].UpdateData<glm::vec3>(0, instances, &positions[0]);

            VAO["sizeVBO"].Bind();
            VAO["sizeVBO"].UpdateData<glm::vec3>(0, instances, &sizes[0]);
        }

        // render data
        VAO.Bind();
        VAO.Draw(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0, instances);
        ArrayObject::Clear();
    }

    void AddInstance(BoundingRegion br, glm::vec3 pos, glm::vec3 size) 
    {
        positions.push_back(br.CalculateCenter() * size + pos);

        sizes.push_back(br.CalculateDimensions() * size);
    }

    void Cleanup() 
    {
        VAO.Cleanup();
    }

private:
    ArrayObject                 VAO;

    std::vector<float>          vertices;
    std::vector<unsigned int>   indices;
};

#endif