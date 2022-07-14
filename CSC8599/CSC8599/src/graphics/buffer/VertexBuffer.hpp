#ifndef VERTEXMEMORY_HPP
#define VERTEXMEMORY_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <map>

/*
    class for buffer objects
    - VBOs, EBOs, etc
*/

class BufferObject 
{
public:
    // value/location
    GLuint val;
    // type of buffer (GL_ARRAY_BUFFER || GL_ELEMENT_ARRAY_BUFFER, etc)
    GLenum type;

    /*
        constructor
    */

    // default and initialize with type
    BufferObject(GLenum type = GL_ARRAY_BUFFER) 
        : type(type) {}

    // generate object
    void Generate() 
    {
        glGenBuffers(1, &val);
    }

    // bind object
    void Bind() 
    {
        glBindBuffer(type, val);
    }

    // set data (glBufferData)
    template<typename T>
    void SetData(GLuint noElements, T* data, GLenum usage) 
    {
        glBufferData(type, noElements * sizeof(T), data, usage);
    }

    // update data (glBufferSubData)
    template<typename T>
    void UpdateData(GLintptr offset, GLuint noElements, T* data) 
    {
        glBufferSubData(type, offset, noElements * sizeof(T), data);
    }

    // set attribute pointers
    template<typename T>
    void SetAttPointer(GLuint idx, GLint size, GLenum type, GLuint stride, GLuint offset, GLuint divisor = 0) 
    {
        glVertexAttribPointer(idx, size, type, GL_FALSE, stride * sizeof(T), (void*)(offset * sizeof(T)));
        glEnableVertexAttribArray(idx);
        if (divisor > 0) 
        {
            // reset _idx_ attribute every _divisor_ iteration (instancing)
            glVertexAttribDivisor(idx, divisor);
        }
    }

    // clear buffer objects (bind 0)
    void Clear() 
    {
        glBindBuffer(type, 0);
    }

    // cleanup
    void Cleanup() 
    {
        glDeleteBuffers(1, &val);
    }
};

/*
    class for array objects
    - VAO
*/
class ArrayObject 
{
public:
    // value/location
    GLuint val;

    // map of names to buffers
    std::map<const char*, BufferObject> buffers;

    // get buffer (override [])
    BufferObject& operator[](const char* key) 
    {
        return buffers[key];
    }

    // generate object
    void Generate() 
    {
        glGenVertexArrays(1, &val);
    }

    // bind
    void Bind() 
    {
        glBindVertexArray(val);
    }

    // draw arrays
    void Draw(GLenum mode, GLuint first, GLuint count) 
    {
        glDrawArrays(mode, first, count);
    }

    // draw
    void Draw(GLenum mode, GLuint count, GLenum type, GLint indices, GLuint instancecount = 1) 
    {
        glDrawElementsInstanced(mode, count, type, (void*)indices, instancecount);
    }

    // cleanup
    void Cleanup() 
    {
        glDeleteVertexArrays(1, &val);
        for (auto& pair : buffers) 
        {
            pair.second.Cleanup();
        }
    }

    // clear array object (bind 0)
    static void Clear() 
    {
        glBindVertexArray(0);
    }
};

#endif