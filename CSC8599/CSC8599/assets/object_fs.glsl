#version 330 core

struct Material
{
	vec3 ambient;
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

struct DirLight
{
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight
{
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
    float linear;
    float quadratic;
};

struct SpotLight
{
	vec3 position;
	vec3 direction;

	float cutOff;
	float outerCutOff;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform DirLight dirLight;
uniform PointLight pointLight;
uniform SpotLight spotLight;
uniform Material material;
uniform vec3 viewPos;

in vec3 fragPos;
in vec3 normal;
in vec2 texCoord;

out vec4 fragColor;

vec3 CalDirLight(vec3 norm, vec3 viewDir, vec3 diffMap, vec3 specMap);
vec3 CalPointLight(vec3 norm, vec3 viewDir, vec3 diffMap, vec3 specMap);
vec3 CalSpotLight(vec3 norm, vec3 viewDir, vec3 diffMap, vec3 specMap);

void main()
{
	vec3 norm		= normalize(normal);
	vec3 viewDir	= normalize(viewPos - fragPos);
	vec3 diffMap	= texture(material.diffuse, texCoord).rgb;
	vec3 specMap	= texture(material.specular, texCoord).rgb;

	vec3 ret1		= CalPointLight(norm, viewDir, diffMap, specMap);
	vec3 ret2		= CalDirLight(norm, viewDir, diffMap, specMap);
	fragColor		= vec4(ret2, 1.0);
}

vec3 CalDirLight(vec3 norm, vec3 viewDir, vec3 diffMap, vec3 specMap)
{
	// Ambient
	vec3 ambient	= dirLight.ambient * diffMap;

	// Diffuse
	vec3 lightDir	= normalize(-dirLight.direction);
	float diff		= max(dot(norm, lightDir), 0.0);
	vec3 diffuse	= dirLight.diffuse * (diff * diffMap);

	// Specular
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec		= pow(max(dot(viewDir, reflectDir), 0.0), material.shininess * 128);
	vec3 specular	= dirLight.specular * (spec * specMap);

	vec3 result		= ambient + diffuse + specular;
	return result;
	
}

vec3 CalPointLight(vec3 norm, vec3 viewDir, vec3 diffMap, vec3 specMap)
{
	// Ambient
	vec3 ambient	= pointLight.ambient * diffMap;

	// Diffuse
	vec3 lightDir	= normalize(pointLight.position - fragPos);
	float diff		= max(dot(norm, lightDir), 0.0);
	vec3 diffuse	= pointLight.diffuse * (diff * diffMap);

	// Specular
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec		= pow(max(dot(viewDir, reflectDir), 0.0), material.shininess * 128);
	vec3 specular	= pointLight.specular * (spec * specMap);

	// Attenuation
    float distance	= length(pointLight.position - fragPos);
    float attenuation = 1.0 / (pointLight.constant + pointLight.linear * distance + pointLight.quadratic * (distance * distance));
	ambient			*= attenuation;  
    diffuse			*= attenuation;
    specular		*= attenuation; 

	vec3 result		= ambient + diffuse + specular;
	return result;
	
}

vec3 CalSpotLight(vec3 norm, vec3 viewDir, vec3 diffMap, vec3 specMap)
{
	// Ambient
	vec3 ambient	= spotLight.ambient * diffMap;

	// Diffuse
	vec3 lightDir	= normalize(spotLight.position - fragPos);
	float diff		= max(dot(norm, lightDir), 0.0);
	vec3 diffuse	= spotLight.diffuse * (diff * diffMap);

	// Specular
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec		= pow(max(dot(viewDir, reflectDir), 0.0), material.shininess * 128);
	vec3 specular	= spotLight.specular * (spec * specMap);

	vec3 result		= ambient + diffuse + specular;
	return result;
}