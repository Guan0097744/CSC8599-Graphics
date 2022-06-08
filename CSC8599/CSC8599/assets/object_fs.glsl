#version 330 core

struct Material
{
	vec4 diffuse;
	vec4 specular;
	float shininess;
};

struct DirLight
{
	vec3 direction;

	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
};

#define MAX_POINT_LIGHTS 20
struct PointLight
{
	vec3 position;

	float constant;
    float linear;
    float quadratic;

	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
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

	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
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
uniform int noTex;

in vec3 fragPos;
in vec3 normal;
in vec2 texCoord;

out vec4 fragColor;

vec4 CalDirLight(vec3 norm, vec3 viewDir, vec4 diffMap, vec4 specMap);
vec4 CalPointLight(int index, vec3 norm, vec3 viewDir, vec4 diffMap, vec4 specMap);
vec4 CalSpotLight(int index, vec3 norm, vec3 viewDir, vec4 diffMap, vec4 specMap);

void main()
{
	vec3 norm		= normalize(normal);
	vec3 viewDir	= normalize(viewPos - fragPos);

	vec4 diffMap;
	vec4 specMap;

	if(noTex == 1)
	{
		diffMap = material.diffuse;
		specMap = material.specular;
	}
	else
	{
		diffMap	= texture(diffuse0, texCoord);
		specMap	= texture(specular0, texCoord);
	}

	vec4 result;

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

	fragColor = result;
}

vec4 CalDirLight(vec3 norm, vec3 viewDir, vec4 diffMap, vec4 specMap)
{
	// Ambient
	vec4 ambient	= dirLight.ambient * diffMap;

	// Diffuse
	vec3 lightDir	= normalize(-dirLight.direction);
	float diff		= max(dot(norm, lightDir), 0.0);
	vec4 diffuse	= dirLight.diffuse * (diff * diffMap);

	// Specular
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec		= pow(max(dot(viewDir, reflectDir), 0.0), material.shininess * 128);
	vec4 specular	= dirLight.specular * (spec * specMap);

	vec4 result		= ambient + diffuse + specular;
	return result;
	
}

vec4 CalPointLight(int index, vec3 norm, vec3 viewDir, vec4 diffMap, vec4 specMap)
{
	// Ambient
	vec4 ambient	= pointLights[index].ambient * diffMap;

	// Diffuse
	vec3 lightDir	= normalize(pointLights[index].position - fragPos);
	float diff		= max(dot(norm, lightDir), 0.0);
	vec4 diffuse	= pointLights[index].diffuse * (diff * diffMap);

	// Specular
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec		= pow(max(dot(viewDir, reflectDir), 0.0), material.shininess * 128);
	vec4 specular	= pointLights[index].specular * (spec * specMap);

	// Attenuation
    float distance	= length(pointLights[index].position - fragPos);
    float attenuation = 1.0 / (pointLights[index].constant + pointLights[index].linear * distance + pointLights[index].quadratic * (distance * distance));
	ambient			*= attenuation;  
    diffuse			*= attenuation;
    specular		*= attenuation; 

	vec4 result		= ambient + diffuse + specular;
	return result;
	
}

vec4 CalSpotLight(int index, vec3 norm, vec3 viewDir, vec4 diffMap, vec4 specMap)
{
	// Ambient
	vec4 ambient	= spotLights[index].ambient * diffMap;

	vec3 lightDir	= normalize(spotLights[index].position - fragPos);
	float theta		= dot(lightDir, normalize(-spotLights[index].direction));
	if(theta > spotLights[index].outerCutOff)
	{
		// Diffuse
		float diff		= max(dot(norm, lightDir), 0.0);
		vec4 diffuse	= spotLights[index].diffuse * (diff * diffMap);

		// Specular
		vec3 reflectDir = reflect(-lightDir, norm);
		float spec		= pow(max(dot(viewDir, reflectDir), 0.0), material.shininess * 128);
		vec4 specular	= spotLights[index].specular * (spec * specMap);

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

		vec4 result		= ambient + diffuse + specular;

		return result;
	}
	else
	{
		return ambient;
	}

}