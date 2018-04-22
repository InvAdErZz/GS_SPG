#version 330 core

out vec4 normalAmbient;

in VS_OUT
{
	vec2 texSpace;
} frag_in;

uniform sampler3D densityTex;

uniform vec3 inversedTexture3dDimensions;
uniform float height;

void main(void) 
{
	vec3 texSpace3d = vec3(frag_in.texSpace,height);
	vec4 step = vec4(inversedTexture3dDimensions, 0);
	vec3 gradient = vec3(
		texture(densityTex, texSpace3d + step.xww).r - texture(densityTex, texSpace3d - step.xww ).r,
		texture(densityTex, texSpace3d + step.wyw).r - texture(densityTex, texSpace3d - step.wyw ).r,
		texture(densityTex, texSpace3d + step.wwz).r - texture(densityTex, texSpace3d - step.wwz ).r
		);
	
	vec3 normalVec = normalize(-gradient);
	normalAmbient = vec4(normalVec,0);	
}