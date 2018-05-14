#version 330 core

in vec3 in_ParticlePosition;
in vec3 in_ParticleVelocity;
in float in_ParticleSecondsToLive;

uniform float deltaSeconds;

out VS_OUT
{
	vec3 Position;
	vec3 Velocity;
	float SecondsToLive;
} vs_out;

void main(void) 
{	
	vs_out.Position = in_ParticlePosition + in_ParticleVelocity * deltaSeconds;
	vs_out.Velocity = in_ParticleVelocity;
	vs_out.SecondsToLive = in_ParticleSecondsToLive - deltaSeconds;		
}