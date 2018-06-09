#version 330 core

out float resultVal;

uniform sampler2D inputTex;
uniform int gausRadius = 1;
uniform bool isHorizontal = false;


const int gauss_samples = 7;
const int gauss_halfSamples = gauss_samples / 2;

const float gauss_weights[gauss_samples]= float[](
	1.f  / 64.f ,
	6.f  / 64.f,
	15.f / 64.f,
	20.f / 64.f,
	15.f / 64.f,
	6.f  / 64.f,
	1.f  / 64.f
 );




void main(void) {
	ivec2 gaussStep = isHorizontal ? ivec2(gausRadius, 0) : ivec2(0, gausRadius);
	ivec2 screenPixelCoords = ivec2(gl_FragCoord.xy );
	
	ivec2 startPosition = screenPixelCoords - gauss_halfSamples * gaussStep;
	float totalValue = 0.f;
	ivec2 currentPosition = startPosition;
	for(int i = 0; i < gauss_samples; ++i)
	{
		totalValue += texelFetch(inputTex,currentPosition, 0).r * gauss_weights[i];
		currentPosition += gaussStep;
	}
	resultVal = totalValue;
	
	
	//resultVal = texelFetch(inputTex,ivec2(gl_FragCoord.xy), 0).r;
}