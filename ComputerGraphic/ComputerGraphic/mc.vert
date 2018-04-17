#version 330 core

in vec3 in_Position;

out VS_OUT
{
   uint mc_case;
   vec3 samplePositions[8];
   float sampleDensity[8];
} vs_out;


uniform float worldSpaceVoxelSize;
uniform sampler3D densityTex;

void main(void) {

	vec2 step = vec2(worldSpaceVoxelSize,0);

	vec3 textureSpace = in_Position;
	
	vs_out.samplePositions[0] =  textureSpace + step.yyy;
	vs_out.samplePositions[1] =  textureSpace + step.yyx;
	vs_out.samplePositions[2] =  textureSpace + step.xyx;
	vs_out.samplePositions[3] =  textureSpace + step.xyy;	
	vs_out.samplePositions[4] =  textureSpace + step.yxy;
	vs_out.samplePositions[5] =  textureSpace + step.yxx;
	vs_out.samplePositions[6] =  textureSpace + step.xxx;
	vs_out.samplePositions[7] =  textureSpace + step.xxy;	
	
	#if 1	
	vs_out.sampleDensity[0] =  (densityTex, vs_out.samplePositions[0]).r;
	vs_out.sampleDensity[1] =  (densityTex, vs_out.samplePositions[1]).r;
	vs_out.sampleDensity[2] =  (densityTex, vs_out.samplePositions[2]).r;
	vs_out.sampleDensity[3] =  (densityTex, vs_out.samplePositions[3]).r;	
	vs_out.sampleDensity[4] =  (densityTex, vs_out.samplePositions[4]).r;
	vs_out.sampleDensity[5] =  (densityTex, vs_out.samplePositions[5]).r;
	vs_out.sampleDensity[6] =  (densityTex, vs_out.samplePositions[6]).r;
	vs_out.sampleDensity[7] =  (densityTex, vs_out.samplePositions[7]).r;	
	#else
	vs_out.sampleDensity[0] =  1.f;
	vs_out.sampleDensity[1] =  2.f;
	vs_out.sampleDensity[2] =  3.f;
	vs_out.sampleDensity[3] =  4.f;
	vs_out.sampleDensity[4] =  5.f;
	vs_out.sampleDensity[5] =  6.f;
	vs_out.sampleDensity[6] = 7.f;
	vs_out.sampleDensity[7] = 	8.f;
	#endif
	
	vs_out.mc_case =  	uint(vs_out.sampleDensity[0] > 0) << 0 |
						uint(vs_out.sampleDensity[1] > 0) << 1 |
						uint(vs_out.sampleDensity[2] > 0) << 2 |
						uint(vs_out.sampleDensity[3] > 0) << 3 |
						uint(vs_out.sampleDensity[4] > 0) << 4 |
						uint(vs_out.sampleDensity[5] > 0) << 5 |
						uint(vs_out.sampleDensity[6] > 0) << 6 |
						uint(vs_out.sampleDensity[7] > 0) << 7 ;
}