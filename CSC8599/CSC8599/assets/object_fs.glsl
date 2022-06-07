#version 330 core

struct Material
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct DirLight
{
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

#define MAX_POINT_LIGHTS 20
struct PointLight
{
	vec3 position;

	float constant;
    float linear;
    float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

#define MAX_SPOT_LIGHTS 5
struct SpotLight
{
	vec3 position;
	vec3 direction;

	float cutOff;
	float outerCutOff;

	float constant;
    float linear;
    float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform sampler2D diffuse0;
uniform sampler2D specular0;
uniform DirLight dirLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];
uniform int numPointLights;
uniform int numSpotLights;
uniform Material material;
uniform vec3 viewPos;

in vec3 fragPos;
in vec3 normal;
in vec2 texCoord;

out vec4 fragColor;

vec3 CalDirLight(vec3 norm, vec3 viewDir, vec3 diffMap, vec3 specMap);
vec3 CalPointLight(int index, vec3 norm, vec3 viewDir, vec3 diffMap, vec3 specMap);
vec3 CalSpotLight(int index, vec3 norm, vec3 viewDir, vec3 diffMap, vec3 specMap);

void main()
{
	vec3 norm		= normalize(normal);
	vec3 viewDir	= normalize(viewPos - fragPos);
	vec3 diffMap	= texture(diffuse0, texCoord).rgb;
	vec3 specMap	= texture(specular0, texCoord).rgb;

	vec3 result;

	// Directional light
	result = CalDirLight(norm, viewDir, diffMap, specMap);

	// Point lights
	for(int i = 0; i < numPointLights; i++)
	{
		result += CalPointLight(i, norm, viewDir, diffMap, specMap);
	}

	// Spot lights
	for(int i = 0; i < numPointLights; i++)
	{
		result += CalSpotLight(i, norm, viewDir, diffMap, specMap);
	}

	fragColor		= vec4(result, 1.0);
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

vec3 CalPointLight(int index, vec3 norm, vec3 viewDir, vec3 diffMap, vec3 specMap)
{
	// Ambient
	vec3 ambient	= pointLights[index].ambient * diffMap;

	// Diffuse
	vec3 lightDir	= normalize(pointLights[index].position - fragPos);
	float diff		= max(dot(norm, lightDir), 0.0);
	vec3 diffuse	= pointLights[index].diffuse * (diff * diffMap);

	// Specular
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec		= pow(max(dot(viewDir, reflectDir), 0.0), material.shininess * 128);
	vec3 specular	= pointLights[index].specular * (spec * specMap);

	// Attenuation
    float distance	= length(pointLights[index].position - fragPos);
    float attenuation = 1.0 / (pointLights[index].constant + pointLights[index].linear * distance + pointLights[index].quadratic * (distance * distance));
	ambient			*= attenuation;  
    diffuse			*= attenuation;
    specular		*= attenuation; 

	vec3 result		= ambient + diffuse + specular;
	return result;
	
}

vec3 CalSpotLight(int index, vec3 norm, vec3 viewDir, vec3 diffMap, vec3 specMap)
{
	// Ambient
	vec3 ambient	= spotLights[index].ambient * diffMap;

	vec3 lightDir	= normalize(spotLights[index].position - fragPos);
	float theta		= dot(lightDir, normalize(-spotLights[index].direction));
	if(theta > spotLights[index].outerCutOff)
	{
		// Diffuse
		float diff		= max(dot(norm, lightDir), 0.0);
		vec3 diffuse	= spotLights[index].diffuse * (diff * diffMap);

		// Specular
		vec3 reflectDir = reflect(-lightDir, norm);
		float spec		= pow(max(dot(viewDir, reflectDir), 0.0), material.shininess * 128);
		vec3 specular	= spotLights[index].specular * (spec * specMap);

		float intensity = (theta - spotLights[index].outerCutOff) / (spotLights[index].cutOff - spotLights[index].outerCutOff);
		intensity = clamp(intensity, 0.0, 1.0);
		diffuse *= intensity;
		specular *= intensity;

		// Attenuation
		float distance	= length(spotLights[index].position - fragPos);
		float attenuation = 1.0 / (spotLights[index].constant + spotLights[index].linear * distance + spotLights[index].quadratic * (distance * distance));
		ambient			*= attenuation;  
		diffuse			*= attenuation;
		specular		*= attenuation; 

		vec3 result		= ambient + diffuse + specular;

		return result;
	}
	else
	{
		return ambient;
	}

}