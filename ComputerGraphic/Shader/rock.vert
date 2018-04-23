#version 330

const int maxLights = 2;

layout (location = 0) in vec3 in_Position;
layout (location = 1) in vec3 in_Normal;
layout (location = 2) in vec2 in_Texcoord;

out VS_OUT
{
	vec3 wsPosition;
	vec3 wsNormal;
	vec2 texCoord;
} vs_out;

uniform mat4 viewProjection;
uniform mat4 model;
uniform mat4 inverseTransposedModelMat;
uniform mat4 lightVP[maxLights];
uniform vec4 color;
uniform vec3 cameraPos;
uniform vec3 lightPos[maxLights];

void main(void) 
{
	vs_out.wsPosition = (model * vec4(in_Position, 1.0)).xyz;
	vs_out.wsNormal =  (inverseTransposedModelMat * vec4(in_Normal, 1.0)).xyz;
	vs_out.texCoord = in_Texcoord;
	gl_Position = viewProjection *  vec4(vs_out.wsPosition, 1.0);	
}