#version 330 core

in vec3 in_Position;

out VS_OUT
{
	vec3 worldSpace;
} vs_out;
 
uniform float height;

const vec2 data[3] = vec2[]
(
  vec2(-1.0f, -1.0f),
  vec2(3.f, -1.f),
  vec2(-1.f, 3.f)
);

void main(void) {
	vs_out.worldSpace = vec3(data[ gl_VertexID ], height);
	gl_Position  = vec4( data[gl_VertexID], 0.0, 1.0);
}