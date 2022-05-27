#version 330 core

struct Material
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct Light
{
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform Material material;
uniform Light light;
uniform vec3 viewPos;

in vec3 fragPos;
in vec3 normal;
in vec2 texCoord;

out vec4 fragColor;

//uniform sampler2D texture0;
//uniform sampler2D texture1;
//uniform float mixVal;

void main()
{
	//fragColor = mix(texture(texture0, texCoord), texture(texture1, texCoord), mixVal);

	// Ambient
	vec3 ambient	= light.ambient * material.ambient;

	// Diffuse
	vec3 norm		= normalize(normal);
	vec3 lightDir	= normalize(light.position - fragPos);
	float diff		= max(dot(norm, lightDir), 0.0);
	vec3 diffuse	= light.diffuse * (diff * material.diffuse);

	// Specular
	vec3 viewDir	= normalize(viewPos - fragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec		= pow(max(dot(viewDir, reflectDir), 0.0), material.shininess * 128);
	vec3 specular	= light.specular * (spec * material.specular);

	fragColor		= vec4(vec3(ambient + diffuse + specular), 1.0);

}