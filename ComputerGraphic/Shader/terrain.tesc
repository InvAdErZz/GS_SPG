#version 400

layout(vertices = 4) out;

uniform float innerTessLevel;
uniform float outerTessLevel;

uniform vec2 tesselationLevel;

in VS_OUT
{
	vec3 wsPos;
	vec2 uv;
} vs_in[];

out TESC_OUT
{
	vec3 wsPos;
	vec2 uv;
} tesc_out[];

void main(){
	tesc_out[gl_InvocationID].wsPos = vs_in[gl_InvocationID].wsPos;
	tesc_out[gl_InvocationID].uv = vs_in[gl_InvocationID].uv;

    if(gl_InvocationID == 0) {
        gl_TessLevelInner[0] = tesselationLevel.x;
        gl_TessLevelInner[1] = tesselationLevel.y;
        gl_TessLevelOuter[0] = tesselationLevel.x;
        gl_TessLevelOuter[1] = tesselationLevel.y;
        gl_TessLevelOuter[2] = tesselationLevel.x;
        gl_TessLevelOuter[3] = tesselationLevel.y;
    }
}