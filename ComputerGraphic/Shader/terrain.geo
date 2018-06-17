#version 400

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform sampler2D heightMap;
uniform float maxHeight;
uniform vec3 terrainNormal;

uniform mat4 viewProjection;

in TESE_OUT
{
	vec3 wsPos;
	vec2 uv;
} tese_in[];

out GEO_OUT
{
	vec3 wsPos;
	vec2 uv;
	vec3 normal;
} geo_out;

void main(){
	
	vec3 p2toP0 = tese_in[2].wsPos - tese_in[0].wsPos;
	vec3 p1ToP0 = tese_in[1].wsPos - tese_in[0].wsPos;
	
	vec3 normal =  normalize(cross(p2toP0, p1ToP0));
	
	for(int i = 0; i < 3; ++i)
	{
		geo_out.wsPos = tese_in[i].wsPos;
		geo_out.uv = tese_in[i].uv;
		geo_out.normal = normal;
		gl_Position = viewProjection * vec4(geo_out.wsPos, 1);
		EmitVertex();
	}
	
	EndPrimitive();
}