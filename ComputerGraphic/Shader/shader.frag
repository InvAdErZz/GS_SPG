#version 330

precision highp float;
 
const int maxLights = 2;
 
out vec3 color;
in vec4 ex_Color;

in vec3 LightDirTangentSpace[maxLights];
in vec4 FragPosLightSpace[maxLights];
in vec3 ViewDirTangentSpace;

in vec2 FragTexCoord;

uniform sampler2D shadowmap[maxLights];
uniform sampler2D normalMap;
uniform bool isLightActive[maxLights];
uniform bool useEsm = false;
uniform vec3 lightPos[maxLights];
uniform vec3 cameraPos;
uniform float ambient = 0.2;
uniform float roughness;

uniform float esmModifier = 10.f;

const vec3 defaultNormal = vec3(0,0,1);

/* 0 means in shadow, 1 means in light*/
float EsmShadowMuliplier(vec4 fragcoordInLightspace, int lightIndex)
{
	vec3 projCoords = fragcoordInLightspace.xyz / fragcoordInLightspace.w;
    projCoords = projCoords * 0.5 + 0.5;
	
	if(projCoords.x < 0.f || projCoords.x > 1.f
	|| projCoords.y < 0.f || projCoords.y > 1.f)
	{
		return 0.f;
	}
	
	/* exp(esmModifier*z)*/
    float esmValue = texture(shadowmap[lightIndex], projCoords.xy).r; 
	
	
	float currentDepth = projCoords.z+ 0.04;
	/* exp(esmModifier*z) * exp(-esmModifier*d) */
    float shadowMultiplier = clamp(esmValue * exp(-esmModifier * currentDepth), 0, 1);

	
	return 1.f-shadowMultiplier;
}

bool IsInShadow(vec4 fragcoordInLightspace, int lightIndex)
{
	vec3 projCoords = fragcoordInLightspace.xyz / fragcoordInLightspace.w;
    projCoords = projCoords * 0.5 + 0.5;
	
	if(projCoords.x < 0.f || projCoords.x > 1.f
	|| projCoords.y < 0.f || projCoords.y > 1.f)
	{
		return true;
	}
 
    float closestDepth = texture(shadowmap[lightIndex], projCoords.xy).r; 
	
    float currentDepth = projCoords.z;
	
	return currentDepth > closestDepth;
}

float IsInShadowAsMultiplier(vec4 fragcoordInLightspace, int lightIndex)
{
	bool bIsInShadow = IsInShadow(fragcoordInLightspace, lightIndex);
	return bIsInShadow ? 0.f : 1.f;
}

vec3 GetNormalTangentSpace()
{	
		
		vec3 NormalMapNormal = normalize(texture(normalMap, FragTexCoord).xyz * 2 - 1);	
		//return NormalMapNormal;
		return normalize(NormalMapNormal * roughness + defaultNormal * (1-roughness));	
}

void main(void) {
	
	float totalLight = ambient;
	vec3 viewDirTS = normalize(ViewDirTangentSpace);
	
	for(int i = 0; i < maxLights; ++i)
	{
		if(isLightActive[i]){
			vec3 normalTS = GetNormalTangentSpace();
			vec3 lightDirTS = normalize(LightDirTangentSpace[i]);
			vec3 reflectDirTS = reflect(lightDirTS, normalTS);
			
			float diffuse = max(0,dot(normalTS, lightDirTS));
			float specular = max(0,dot(viewDirTS, reflectDirTS));
			specular = pow(specular, 20);
			float AllLightComponents = diffuse + specular;
			float shadowMultiplier = useEsm ? EsmShadowMuliplier(FragPosLightSpace[i], i) : IsInShadowAsMultiplier(FragPosLightSpace[i], i);
			AllLightComponents *= shadowMultiplier;
			totalLight += AllLightComponents;			
		}
	}
	color = ex_Color.xyz * (totalLight);
}
