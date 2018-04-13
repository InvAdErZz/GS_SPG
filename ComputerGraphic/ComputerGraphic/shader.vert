#version 330

const int maxLights = 2;

layout (location = 0) in vec3 in_Position; // the position variable has attribute position 0
layout (location = 1) in vec3 in_Normal;
layout (location = 2) in vec3 in_Tangent;
layout (location = 3) in vec3 in_Bitangent;
layout (location = 4) in vec2 in_Texcoord;
 
out vec4 ex_Color;
out vec3 LightDirTangentSpace[maxLights];
out vec4 FragPosLightSpace[maxLights];
out vec3 ViewDirTangentSpace;
out vec2 FragTexCoord;


uniform mat4 viewProjection;
uniform mat4 model;
uniform mat4 inverseTransposedModelMat;
uniform mat4 lightVP[maxLights];
uniform vec4 color;
uniform vec3 cameraPos;
uniform vec3 lightPos[maxLights];

void main(void) 
{
	vec3 NormalWorldSpace = normalize((inverseTransposedModelMat * vec4(in_Normal,0.0)).xyz);
	vec3 TangentWorldSpace = normalize((inverseTransposedModelMat * vec4(in_Tangent,0.0)).xyz);
	vec3 BitangentWorldSpace = normalize((inverseTransposedModelMat * vec4(in_Bitangent,0.0)).xyz);
	
	mat3 WorldToTangentSpaceMat = transpose(mat3(
        TangentWorldSpace,
        BitangentWorldSpace,
        NormalWorldSpace
    )); 

	vec3 PositionWorldSpace = (model * vec4(in_Position, 1.0)).xyz;
	vec3 ViewDirWorldSpace = PositionWorldSpace - cameraPos;
	
	ViewDirTangentSpace = WorldToTangentSpaceMat * ViewDirWorldSpace;

	gl_Position = viewProjection *  vec4(PositionWorldSpace, 1.0);
	
	for(int i = 0; i < maxLights; ++i)
	{
		vec3 LightDirWS = lightPos[i] - PositionWorldSpace;	
		LightDirTangentSpace[i] = WorldToTangentSpaceMat * LightDirWS;
		FragPosLightSpace[i] =  lightVP[i] * vec4(PositionWorldSpace, 1.0);
	}	
	ex_Color = color;
	FragTexCoord = in_Texcoord;
}