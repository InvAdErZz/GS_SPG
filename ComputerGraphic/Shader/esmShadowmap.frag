#version 330 core

out float esmDepth;

uniform float esmModifier = 80.f;
void main(void) 
{
	float depth =gl_FragCoord.z;
	esmDepth = exp(depth * esmModifier);
}