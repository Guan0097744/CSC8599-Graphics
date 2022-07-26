#ifndef VERTEXMEMORY_HPP
#define VERTEXMEMORY_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <map>

//============================================================================================//
//Class for buffer objects: VBOs, EBOs, etc
//============================================================================================//

class BufferObject 
{
public:
    GLuint val;     // Value/Location
    GLenum type;    // Buffer Type (GL_ARRAY_BUFFER || GL_ELEMENT_ARRAY_BUFFER, etc)

    BufferObject(GLenum type = GL_ARRAY_BUFFER) 
        : type(type) {}

    /**
     * @brief Generate Object: glGenBuffers()
    */
    void Generate() 
    {
        glGenBuffers(1, &val);
    }

    /**
     * @brief Bind Object: glBindBuffer()
    */
    void Bind() 
    {
        glBindBuffer(type, val);
    }

    /**
     * @brief Set data: glBufferData()
     * @tparam T 
     * @param numElements 
     * @param data 
     * @param usage 
    */
    template<typename T>
    void SetData(GLuint numElements, T* data, GLenum usage) 
    {
        glBufferData(type, numElements * sizeof(T), data, usage);
    }

    /**
     * @brief Update data: glBufferSubData()
     * @tparam T 
     * @param offset 
     * @param numElements 
     * @param data 
    */
    template<typename T>
    void UpdateData(GLintptr offset, GLuint numElements, T* data)
    {
        glBufferSubData(type, offset, numElements * sizeof(T), data);
    }

    /**
     * @brief Set attribute pointers: glVertexAttribPointer(), glEnableVertexAttribArray(), glVertexAttribDivisor()
     * @tparam T 
     * @param idx 
     * @param size 
     * @param type 
     * @param stride 
     * @param offset 
     * @param divisor 
    */
    template<typename T>
    void SetAttPointer(GLuint idx, GLint size, GLenum type, GLuint stride, GLuint offset, GLuint divisor = 0) 
    {
        glVertexAttribPointer(idx, size, type, GL_FALSE, stride * sizeof(T), (void*)(offset * sizeof(T)));
        glEnableVertexAttribArray(idx);
        if (divisor > 0) 
        {
            // Reset idx attribute every divisor iteration (instancing)
            glVertexAttribDivisor(idx, divisor);
        }
    }

    /**
     * @brief Clear buffer objects: glBindBuffer(type, 0);
    */
    void Clear() 
    {
        glBindBuffer(type, 0);
    }

    /**
     * @brief glDeleteBuffers()
    */
    void Cleanup() 
    {
        glDeleteBuffers(1, &val);
    }
};

/**
 * @brief Class for array objects: VAO
*/
class ArrayObject 
{
public:
    GLuint val;

    std::map<const char*, BufferObject> buffers;    // Map of names to buffers

    /**
     * @brief Get buffer (override [])
     * @param key 
     * @return 
    */
    BufferObject& operator[](const char* key) 
    {
        return buffers[key];
    }

    /**
     * @brief Generate Object: glGenVertexArrays()
    */
    void Generate() 
    {
        glGenVertexArrays(1, &val);
    }

    /**
     * @brief Bind: glBindVertexArray()
    */
    void Bind() 
    {
        glBindVertexArray(val);
    }

    /**
     * @brief Draw Arrays: glDrawArrays()
     * @param mode 
     * @param first 
     * @param count 
    */
    void Draw(GLenum mode, GLuint first, GLuint count) 
    {
        glDrawArrays(mode, first, count);
    }

    /**
     * @brief Draw Elements: glDrawElementsInstanced()
     * @param mode 
     * @param count 
     * @param type 
     * @param indices 
     * @param instancecount 
    */
    void Draw(GLenum mode, GLuint count, GLenum type, GLint indices, GLuint instancecount = 1) 
    {
        glDrawElementsInstanced(mode, count, type, (void*)indices, instancecount);
    }

    /**
     * @brief Clear array object: glBindVertexArray(0)
    */
    static void Clear() 
    {
        glBindVertexArray(0);
    }

    /**
     * @brief glDeleteVertexArrays()
    */
    void Cleanup() 
    {
        glDeleteVertexArrays(1, &val);
        for (auto& pair : buffers) 
        {
            pair.second.Cleanup();
        }
    }
};

#endif