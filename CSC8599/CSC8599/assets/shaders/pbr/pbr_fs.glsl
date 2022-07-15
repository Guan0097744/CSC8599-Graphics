#version 330 core
precision highp float;
in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;

// material parameters
uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;

// IBL
//uniform samplerCube irradianceMap;
//uniform samplerCube radianceMap;


// lights
#define POINT_LIGHT_NUMBER 3
uniform vec3 pointLightPositions[POINT_LIGHT_NUMBER];
uniform vec3 pointLightColors[POINT_LIGHT_NUMBER];

uniform vec3 directionLightDir;
uniform vec3 directionLightColor;

uniform vec3 camPos;

out vec4 FragColor;

const float PI = 3.14159265359;

#define MEDIUMP_FLT_MAX    65504.0
#define MEDIUMP_FLT_MIN    0.00006103515625
#define saturateMediump(x) min(x, MEDIUMP_FLT_MAX)
// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	//        float a = roughness*roughness;
	//        float a2 = a*a;
	//        float NdotH = max(dot(N, H), 0.0);
	//        float NdotH2 = NdotH*NdotH;
	//
	//        float nom   = a2;
	//        float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	//        denom = PI * denom * denom;
	//
	//        return saturateMediump(nom / denom);

	// better ndf with spot light shape
	vec3 NxH = cross(N, H);
	float oneMinusNoHSquared = dot(NxH, NxH);
	float NoH = max(dot(N, H), 0.0);
	float a = NoH * roughness;
	float k = roughness / (oneMinusNoHSquared + a * a);
	float d = k * k * (1.0 / PI);
//	return saturateMediump(d,MEDIUMP_FLT_MAX);
	return min(d, MEDIUMP_FLT_MAX);
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r*r) / 8.0;

	float nom   = NdotV;
	float denom = NdotV * (1.0 - k) + k;
//	return saturateMediump(nom / denom,MEDIUMP_FLT_MAX);
	return min(nom / denom, MEDIUMP_FLT_MAX);
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);
//	saturateMediump(ggx1 * ggx2,MEDIUMP_FLT_MAX);
	return min(ggx1 * ggx2, MEDIUMP_FLT_MAX);
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
	return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 EnvDFGLazarov( vec3 specularColor, float gloss, float ndotv ) {
	//# [ Lazarov 2013, "Getting More Physical in Call of Duty: Black Ops II" ]
	//# Adaptation to fit our G term.
	vec4 p0 = vec4( 0.5745, 1.548, -0.02397, 1.301 );
	vec4 p1 = vec4( 0.5753, -0.2511, -0.02066, 0.4755 );
	vec4 t = gloss * p0 + p1;
	float bias = clamp( t.x * min( t.y, exp2( -7.672 * ndotv ) ) + t.z, 0.0, 1.0);
	float delta = clamp( t.w, 0.0, 1.0);
	float scale = delta - bias;
	bias *= clamp( 50.0 * specularColor.y, 0.0, 1.0);
	return specularColor * scale + bias;
}

void main()
{
	vec3 albedo = pow(texture(albedoMap, TexCoords).rgb, vec3(2.2));
	float metallic = texture(metallicMap, TexCoords).r;
	float roughness = texture(roughnessMap, TexCoords).r;
	//        float ao = texture(aoMap, TexCoords).r;
	float ao = 1.0;

	//        vec3 N = getNormalFromMap();
	vec3 N = normalize(Normal);
	vec3 V = normalize(camPos - WorldPos);
	vec3 R = reflect(-V, N);

	// calculate reflectance at normal incidence; if dia-electric (like plastic) use F0
	// of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)
	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, metallic);

	// reflectance equation
	vec3 Lo = vec3(0.0);
	// point light
	for(int i = 0; i < POINT_LIGHT_NUMBER; ++i) {
		// calculate per-light radiance
		vec3 L = normalize(pointLightPositions[i] - WorldPos);
		vec3 H = normalize(V + L);
		float distance = length(pointLightPositions[i] - WorldPos);
		float attenuation = 1.0 / (distance * distance);
		vec3 radiance = pointLightColors[i] * attenuation;

		// Cook-Torrance BRDF
		float NDF = DistributionGGX(N, H, roughness);
		float G   = GeometrySmith(N, V, L, roughness);
		vec3 F    = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);

		vec3 nominator    = NDF * G * F;
		float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
		vec3 specular = nominator / max(denominator, 0.001); // prevent divide by zero for NdotV=0.0 or NdotL=0.0

		// kS is equal to Fresnel
		vec3 kS = F;
		// for energy conservation, the diffuse and specular light can't
		// be above 1.0 (unless the surface emits light); to preserve this
		// relationship the diffuse component (kD) should equal 1.0 - kS.
		vec3 kD = vec3(1.0) - kS;
		// multiply kD by the inverse metalness such that only non-metals
		// have diffuse lighting, or a linear blend if partly metal (pure metals
		// have no diffuse light).
		kD *= 1.0 - metallic;

		// scale light by NdotL
		float NdotL = max(dot(N, L), 0.0);

		// add to outgoing radiance Lo
		Lo += (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
	}
	// directional light
	{
		vec3 L = normalize(-directionLightDir);
		vec3 H = normalize(V + L);
		vec3 radiance = directionLightColor;

		// Cook-Torrance BRDF
		float NDF = DistributionGGX(N, H, roughness);
		float G   = GeometrySmith(N, V, L, roughness);
		vec3 F    = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);

		vec3 nominator    = NDF * G * F;
		float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
		vec3 specular = nominator / max(denominator, 0.001); // prevent divide by zero for NdotV=0.0 or NdotL=0.0

		// kS is equal to Fresnel
		vec3 kS = F;
		// for energy conservation, the diffuse and specular light can't
		// be above 1.0 (unless the surface emits light); to preserve this
		// relationship the diffuse component (kD) should equal 1.0 - kS.
		vec3 kD = vec3(1.0) - kS;
		// multiply kD by the inverse metalness such that only non-metals
		// have diffuse lighting, or a linear blend if partly metal (pure metals
		// have no diffuse light).
		kD *= 1.0 - metallic;

		// scale light by NdotL
		float NdotL = max(dot(N, L), 0.0);

		// add to outgoing radiance Lo
		Lo += (kD * albedo + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
	}

	// ambient lighting (note that the next IBL tutorial will replace
	// this ambient lighting with environment lighting).
	vec3 ambient = vec3(0.1,0.1,0.1);
	vec3 ambientColor = ambient * albedo * ao;

	vec3 color = ambientColor + Lo;

	// HDR tonemapping
	color = color / (color + vec3(1.0));
	// gamma correct
	color = pow(color, vec3(1.0/2.2));

	FragColor = vec4(color, 1.0);
}
