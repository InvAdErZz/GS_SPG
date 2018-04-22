#version 330

const int maxLights = 2;

layout (location = 0) in vec3 in_Position;
layout (location = 1) in vec3 in_Normal;

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
	vec3 worldspace = (model * vec4(in_Position, 1.0)).xyz;
	gl_Position = viewProjection *  vec4(worldspace, 1.0);	
}