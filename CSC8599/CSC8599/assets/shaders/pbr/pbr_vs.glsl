#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 4) in mat4 model;
layout (location = 8) in mat3 normalModel;

out vec2 TexCoord;
out vec3 WorldPos;
out vec3 Normal;

uniform mat4 projection;
uniform mat4 view;
//uniform mat4 model;

void main()
{
    TexCoord   = aTexCoord;
    WorldPos    = vec3(model * vec4(aPos, 1.0));
    Normal      = mat3(model) * aNormal;

    gl_Position =  projection * view * vec4(WorldPos, 1.0);
}