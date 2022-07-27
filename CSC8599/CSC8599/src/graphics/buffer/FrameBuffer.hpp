#ifndef FRAMEMEMORY_HPP
#define FRAMEMEMORY_HPP

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>

#include "../Texture.h"
#include "../Cubemap.h"

class RenderbufferObject 
{
public:
    GLuint val;

    void Generate() 
    {
        glGenRenderbuffers(1, &val);
    }

    void Bind() 
    {
        glBindRenderbuffer(GL_RENDERBUFFER, val);
    }

    void Allocate(GLenum format, GLuint width, GLuint height) 
    {
        glRenderbufferStorage(GL_RENDERBUFFER, format, width, height);
    }

    void Cleanup() 
    {
        glDeleteRenderbuffers(1, &val);
    }
};

class FramebufferObject 
{
public:
    GLuint                  val;
    GLuint                  width;
    GLuint                  height;
    GLbitfield              bitCombo;

    std::vector<GLuint>     rbos;
    std::vector<Texture>    textures;
    Cubemap cubemap;

    FramebufferObject()
        : val(0), width(0), height(0), bitCombo(0) {}

    FramebufferObject(GLuint width, GLuint height, GLbitfield bitCombo)
        : val(0), width(width), height(height), bitCombo(bitCombo) {}

    void Generate() 
    {
        glGenFramebuffers(1, &val);
    }

    void DisableColorBuffer() 
    {
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }

    void Bind() 
    {
        glBindFramebuffer(GL_FRAMEBUFFER, val);
    }

    void SetViewport() 
    {
        glViewport(0, 0, width, height);
    }

    void Clear() 
    {
        glClear(bitCombo);
    }

    void Activate() 
    {
        Bind();
        SetViewport();
        Clear();
    }

    void AllocateAndAttachRBO(GLenum attachType, GLenum format, GLuint w = 512, GLuint h = 512)
    {
        GLuint rbo;

        // generate
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);

        // attach
        glRenderbufferStorage(GL_RENDERBUFFER, format, w, h);
        //glRenderbufferStorage(GL_RENDERBUFFER, format, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachType, GL_RENDERBUFFER, rbo);

        // add to list
        rbos.push_back(rbo);
    }

    void AllocateAndAttachTexture(GLenum attachType, GLenum format, GLenum type) 
    {
        std::string name = "tex" + textures.size();
        Texture tex(name);

        // allocate
        tex.Bind();
        tex.Allocate(format, width, height, type);
        Texture::SetParams(GL_NEAREST, GL_NEAREST, GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER);

        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

        // attach
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachType, GL_TEXTURE_2D, tex.id, 0);

        textures.push_back(tex);
    }

    void AttachTexture(GLenum attachType, Texture tex) 
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER, attachType, GL_TEXTURE_2D, tex.id, 0);
    }

    void AllocateAndAttachCubemap(GLenum attachType, GLenum format, GLenum type) 
    {
        cubemap = Cubemap();

        cubemap.Generate();
        cubemap.Bind();
        cubemap.Allocate(format, width, height, type);

        glFramebufferTexture(GL_FRAMEBUFFER, attachType, cubemap.id, 0);
    }

    void Cleanup() 
    {
        // Delete RBOs
        glDeleteRenderbuffers(rbos.size(), &rbos[0]);

        // Delete generated textures
        for (Texture t : textures) 
        {
            t.Cleanup();
        }

        // Delete FBO
        glDeleteFramebuffers(1, &val);
    }
};

#endif