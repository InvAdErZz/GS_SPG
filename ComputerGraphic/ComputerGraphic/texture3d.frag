#version 330 core

out float finalDensity;
in vec2 screenPos;

uniform float height;

void main(void) {
	float density = 15;
	density -= pow(length(screenPos.xy), 3);
	density +=  cos(height);
	
	finalDensity = density;
}