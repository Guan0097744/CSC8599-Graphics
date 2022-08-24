#ifndef PBRPLANE_H
#define PBRPLANE_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stb/stb_image.h>

#include <string>
#include <vector>

#include "../graphics/Shader.h"
#include "../graphics/buffer/FrameBuffer.hpp"

class PBRPlane
{
public:
    void Render()
    {
        if (VAO.val == 0)
        {
            float quadVertices[] = {
                // positions        // texture Coords
                -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
                -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
                 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            };

            // setup plane VAO
            VAO.Generate();
            VAO.Bind();

            // setup VBO
            VAO["VBO"] = BufferObject(GL_ARRAY_BUFFER);
            VAO["VBO"].Generate();
            VAO["VBO"].Bind();
            VAO["VBO"].SetData<float>(4 * 5, quadVertices, GL_STATIC_DRAW);

            // Vertex Positions
            VAO["VBO"].SetAttPointer<GLfloat>(0, 3, GL_FLOAT, 5, 0);
            // Vertex texture coords
            VAO["VBO"].SetAttPointer<GLfloat>(1, 2, GL_FLOAT, 5, 3);

            VAO["VBO"].Clear();

            ArrayObject::Clear();
        }

        VAO.Bind();
        VAO.Draw(GL_TRIANGLE_STRIP, 0, 4);
        ArrayObject::Clear();
    }

private:
    ArrayObject VAO;
};

#endif