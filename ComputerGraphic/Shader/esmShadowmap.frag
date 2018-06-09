#version 330 core

out float esmDepth;

uniform float esmModifier = 10.f;
void main(void) {

	esmDepth = exp( gl_FragCoord.z * esmModifier);
}