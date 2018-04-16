#version 330 core

in vec3 in_Position;

out VS_OUT
{
   uint mc_case;
   vec3[8] samplePositions;
   float[8] sampleDensity;
} vs_out;


uniform float worldSpaceVoxelSize;
uniform sampler3D densityTex;

void main(void) {

	vec2 step = vec2(worldSpaceVoxelSize,0);

	vec3 textureSpace = in_Position;

	vec4 density0123 = vec4( 
		texture (densityTex, textureSpace + step.yyy).r,
		texture (densityTex, textureSpace + step.yyx).r,
		texture (densityTex, textureSpace + step.xyx).r,
		texture (densityTex, textureSpace + step.xyy).r		
	);
	vec4 density4567 = vec4( 
		texture (densityTex, textureSpace + step.yxy).r,
		texture (densityTex, textureSpace + step.yxx).r,
		texture (densityTex, textureSpace + step.xxx).r,
		texture (densityTex, textureSpace + step.xxy).r		
	);
	
	vec4 zeroVec = vec4(0,0,0,0);
	bvec4 b0123 = greaterThan(density0123, zeroVec);
	bvec4 b4567 = greaterThan(density4567, zeroVec);
	
	vs_out.mc_case =  	uint(b0123.x) << 0 |
						uint(b0123.y) << 1 |
						uint(b0123.z) << 2 |
						uint(b0123.w) << 3 |

						uint(b4567.x) << 4 |
						uint(b4567.y) << 5 |
						uint(b4567.z) << 6 |
						uint(b4567.w) << 7 ;
}