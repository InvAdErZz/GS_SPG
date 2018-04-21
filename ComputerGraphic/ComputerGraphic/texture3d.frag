#version 330 core

out float finalDensity;

in VS_OUT
{
	vec3 worldSpace;
	flat int id;
} frag_in;


void main(void) {
	
	float height = frag_in.worldSpace.z;
	float density = 20;
	density -= pow(length(frag_in.worldSpace.xy)*5, 3);
	density +=  cos(height);
	
	finalDensity = height * 256;
	finalDensity = frag_in.id;
	
	finalDensity = height;
	//finalDensity = 15;
}