#version 400

layout(vertices = 4) out;

uniform float innerTessLevel;
uniform float outerTessLevel;

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
        gl_TessLevelInner[0] = innerTessLevel;
        gl_TessLevelInner[1] = innerTessLevel;
        gl_TessLevelOuter[0] = outerTessLevel;
        gl_TessLevelOuter[1] = outerTessLevel;
        gl_TessLevelOuter[2] = outerTessLevel;
        gl_TessLevelOuter[3] = outerTessLevel;
    }
}