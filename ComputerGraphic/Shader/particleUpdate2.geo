#version 330 core

layout(points) in;
layout(points, max_vertices = 1) out;

in VS_OUT
{
	vec3 Position;
	vec3 Velocity;
	float SecondsToLive;
} geo_in[];

struct Particle
{
	vec3 Position;
	vec3 Velocity;
	float SecondsToLive;
};

out Particle geo_out;

void main()
{
	if(geo_in[0].SecondsToLive > 0)
	{
		geo_out.Position = geo_in[0].Position;
		geo_out.Velocity = geo_in[0].Velocity;
		geo_out.SecondsToLive = geo_in[0].SecondsToLive;
		
		EmitVertex();
		EndPrimitive();	
	}	
}