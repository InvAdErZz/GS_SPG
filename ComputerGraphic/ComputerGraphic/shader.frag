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
uniform vec3 lightPos[maxLights];
uniform vec3 cameraPos;
uniform float ambient = 0.2;
uniform float roughness;

const vec3 defaultNormal = vec3(0,0,1);


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
			if(!IsInShadow(FragPosLightSpace[i], i))
			{
				vec3 normalTS = GetNormalTangentSpace();
				vec3 lightDirTS = normalize(LightDirTangentSpace[i]);
				vec3 reflectDirTS = reflect(lightDirTS, normalTS);
				
				float diffuse = max(0,dot(normalTS, lightDirTS));
				float specular = max(0,dot(viewDirTS, reflectDirTS));
				specular = pow(specular, 20);
				totalLight += diffuse +
				specular;
			}
		}
	}
	color = ex_Color.xyz * (totalLight);
}
