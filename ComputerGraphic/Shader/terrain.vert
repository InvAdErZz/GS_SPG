#version 400

uniform vec2 halfExtent;
uniform vec3 center;
uniform vec3 terrainNormal;
uniform vec3 terrainTangent;
uniform vec3 terrainBitangent;

const vec2 normalizedPos[4] = vec2[]
(
  vec2( 1.0, 1.0),
  vec2(-1.0, 1.0),
  vec2( 1.0,-1.0),
  vec2(-1.0,-1.0)
);

const vec2 uvCoords[4] = vec2[]
(
  vec2( 1.0, 1.0),
  vec2( 0.0, 1.0),
  vec2( 1.0, 0.0),
  vec2( 0.0, 0.0)
);

out VS_OUT
{
	vec3 wsPos;
	vec2 uv;
} vs_out;

void main(void) 
{
	vec3 wsPositon = center 
		+ normalizedPos[gl_VertexID].x * halfExtent.x * terrainTangent
		+ normalizedPos[gl_VertexID].y * halfExtent.y * terrainBitangent;
	
	vs_out.wsPos = wsPositon;
	vs_out.uv = uvCoords[gl_VertexID];
}