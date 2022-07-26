#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;
//layout (location = 4) in mat4 model;
layout (location = 8) in mat3 normalModel;

out vec3 tNormal;
out	vec3 tFragPos;
out	vec3 tViewPos;

out vec3 FragPos;
out vec2 TexCoord;
out vec3 WorldPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 viewPos;

// DirLight

void main()
{
	// get position in world space
	// apply model transformation
	FragPos					= vec3(model * vec4(aPos, 1.0));

	// determine normal vector in tangent space
	tNormal					= normalize(normalModel * aNormal);

	// calculate tangent space matrix
	vec3 T					= normalize(normalModel * aTangent);
	vec3 N					= tNormal;
	T						= normalize(T - dot(T, N) * N);		// re-orthogonalize T with respect to N
	vec3 B					= cross(N, T);						// get B, perpendicular to N and T
	mat3 TBNinv				= transpose(mat3(T, B, N));			// orthogonal matrix => transpose = inverse

	// transform positions to the tangent space
	tFragPos				= TBNinv * tFragPos;
	tViewPos				= TBNinv * viewPos;

	TexCoord				= aTexCoord;
    WorldPos				= vec3(model * vec4(aPos, 1.0));
    Normal					= mat3(model) * aNormal;

	// set output for fragment shader
	//gl_Position				= projection * view * vec4(FragPos, 1.0);
    gl_Position				=  projection * view * vec4(WorldPos, 1.0);
}
