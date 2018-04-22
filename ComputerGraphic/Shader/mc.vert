#version 330 core

layout (location = 0) in vec2 in_Position;

out VS_OUT
{
   uint mc_case;
   vec3 samplePositions[8];
   float sampleDensity[8];
} vs_out;


uniform vec3 inversedTexture3dDimensions;

uniform sampler3D densityTex;

void main(void) {		
	
	
	ivec2 step = ivec2(1,0);
	
	ivec3 texSamplePos[8];
	texSamplePos[0] = ivec3(in_Position,gl_InstanceID);
	texSamplePos[1] =  texSamplePos[0] + step.yyx;
	texSamplePos[2] =  texSamplePos[0] + step.xyx;
	texSamplePos[3] =  texSamplePos[0] + step.xyy;	
	texSamplePos[4] =  texSamplePos[0] + step.yxy;
	texSamplePos[5] =  texSamplePos[0] + step.yxx;
	texSamplePos[6] =  texSamplePos[0] + step.xxx;
	texSamplePos[7] =  texSamplePos[0] + step.xxy;	
	
	for(int i = 0; i <8 ; ++i)
	{
		vs_out.sampleDensity[i] =  texelFetch(densityTex, texSamplePos[i], 0).r;
		vs_out.samplePositions[i] =  texSamplePos[i] * inversedTexture3dDimensions;
	}	
	
	gl_Position = vec4(vs_out.samplePositions[0],1);

	vs_out.mc_case =  	uint(vs_out.sampleDensity[0] > 0) << 0 |
						uint(vs_out.sampleDensity[1] > 0) << 1 |
						uint(vs_out.sampleDensity[2] > 0) << 2 |
						uint(vs_out.sampleDensity[3] > 0) << 3 |
						uint(vs_out.sampleDensity[4] > 0) << 4 |
						uint(vs_out.sampleDensity[5] > 0) << 5 |
						uint(vs_out.sampleDensity[6] > 0) << 6 |
						uint(vs_out.sampleDensity[7] > 0) << 7 ;	
	
}