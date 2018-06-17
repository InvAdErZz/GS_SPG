#version 400

layout(quads, equal_spacing, ccw) in;


uniform sampler2D heightMap;
uniform float maxHeight;
uniform vec3 terrainNormal;

uniform mat4 viewProjection;

in TESC_OUT
{
	vec3 wsPos;
	vec2 uv;
} tesc_in[];

out TESE_OUT
{
	vec3 wsPos;
	vec2 uv;
} tese_out;

void main(){
	
	// bilinear interpolate to get position
	vec3 wsPosPoint1 = mix(tesc_in[0].wsPos, tesc_in[1].wsPos, gl_TessCoord.x);
	vec3 wsPosPoint2 = mix(tesc_in[2].wsPos, tesc_in[3].wsPos, gl_TessCoord.x);
	vec3 wsPos = mix(wsPosPoint1,wsPosPoint2,gl_TessCoord.y);
		
	vec2 uvPoint1 = mix(tesc_in[0].uv, tesc_in[1].uv, gl_TessCoord.x);
	vec2 uvPoint2 = mix(tesc_in[2].uv, tesc_in[3].uv, gl_TessCoord.x);
	vec2 uv = mix(uvPoint1,uvPoint2,gl_TessCoord.y);

    float normalizedHeight = texture(heightMap, uv).r;
	float height = normalizedHeight * maxHeight;
   // wsPos += terrainNormal * height;
	
	tese_out.wsPos = wsPos;
	tese_out.uv = uv;
}