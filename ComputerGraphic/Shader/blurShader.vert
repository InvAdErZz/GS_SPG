#version 330 core

const vec2 data[3] = vec2[]
(
  vec2(-1.0f, -1.0f),
  vec2(3.f, -1.f),
  vec2(-1.f, 3.f)
);

void main(void) {
	gl_Position  = vec4( data[gl_VertexID], 0.0, 1.0);
}