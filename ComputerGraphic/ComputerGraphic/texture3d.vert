#version 330 core

in vec3 in_Position;
out vec2 screenPos;

void main(void) {
	vec2 position2d = vec2(in_Position.x, in_Position.y);
	gl_Position = vec4(position2d, 0.0, 1.0);
	screenPos = position2d;
}