#version 330 core

in vec3 in_Position;

out VS_OUT
{
	vec3 worldSpace;
	flat int id;
} vs_out;
 
uniform float height;

void main(void) {
	vec2 position2d = vec2(in_Position.x, in_Position.y);
	
	// we have 256 layers
	vs_out.id = gl_InstanceID;
	//float height = gl_InstanceID / 256.f;
	
	vs_out.worldSpace = vec3(position2d, height);
	
	gl_Position = vec4(position2d, 0.0, 1.0);
}