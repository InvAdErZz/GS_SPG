#version 330

// partilces
in vec3 in_ParticlePosition;
in vec3 in_ParticleVelocity;
in float in_ParticleSecondsToLive;

// particle mesh
in vec3 in_Position;
in vec3 in_Normal;
in vec3 in_Tangent;
in vec3 in_Bitangent;
in vec2	in_Texcoord;

out VS_OUT
{
	vec3 Position;
	vec3 Normal;
	vec3 Color;
} vs_out;

uniform mat4 viewProjection;
uniform mat4 model;
uniform mat4 inverseTransposedModelMat;

void main(void) 
{
	vec3 positionInParticleSystem = in_Position + in_ParticlePosition;
	
	vs_out.Position = positionInParticleSystem; //(vec4(positionInParticleSystem,1) * model).xyz;
	vs_out.Normal = in_Normal;//(vec4(positionInParticleSystem,0) * inverseTransposedModelMat).xyz;
	vs_out.Color = mix(vec3(1,0,0), vec3(0,1,0), in_ParticleSecondsToLive / 3.f);
	
	gl_Position = viewProjection *  vec4(vs_out.Position, 1.0);	
}