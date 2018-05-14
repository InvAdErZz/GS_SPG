#version 330

precision highp float;
 
in VS_OUT
{
	vec3 Position;
	vec3 Normal;
	vec3 Color;
} frag_in;
 
out vec3 color;


void main(void) {
	color = (frag_in.Color + frag_in.Normal) / 2;
}
