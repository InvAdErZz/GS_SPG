#version 330 core

in vec3 in_Position;
 
uniform mat4 viewProjection;
uniform mat4 model;
uniform vec4 color;

void main(void) {
 
	gl_Position = viewProjection *model* vec4(in_Position.x, in_Position.y, in_Position.z, 1.0);
}