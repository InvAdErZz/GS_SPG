#version 330 core

out VS_OUT
{
	vec2 texSpace;
} vs_out;

const vec2 screenTriangle[3] = vec2[]
(
  vec2(-1.0f, -1.0f),
  vec2(3.f, -1.f),
  vec2(-1.f, 3.f)
);

void main(void) 
{
	vs_out.texSpace = vec2(screenTriangle[gl_VertexID]);
	gl_Position = vec4(screenTriangle[gl_VertexID], 0.0, 1.0);
}