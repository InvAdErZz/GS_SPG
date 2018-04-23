#version 330 core

out float finalDensity;

in VS_OUT
{
	vec3 worldSpace;
} frag_in;

void main(void) {
	
	/*
	float height = frag_in.worldSpace.z;
	float density = 10;
	density -= pow(length(frag_in.worldSpace.xy)*2, 3);
	density +=  cos(height) * 10;
	*/
	
	float density = +7;
	density += cos(frag_in.worldSpace.z/ 10);
	density -= 10 / (length(frag_in.worldSpace.xy) + 1);
	
	finalDensity = density ;
	
}