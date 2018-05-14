#version 330

layout (location = 0) in vec3 in_Position;

uniform mat4 viewProjection;

uniform vec4 color;
out vec4 ex_Color;

void main(void) 
{
    gl_Position = viewProjection *  vec4(in_Position, 1.0);
	//gl_Position = vec4(in_Position, 1.0);
}