#version 330 core

out float esmDepth;

uniform float ExpModifier = 1.f;
void main(void) {

	esmDepth = exp( gl_FragCoord.z * ExpModifier);
}