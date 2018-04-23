#version 330 core
const float pi = 3.14;


out float finalDensity;

in VS_OUT
{
	vec3 worldSpace;
} frag_in;






const int numPillars = 3;

const vec2 pillars[numPillars] = vec2[]
(
  vec2(-0.5f, 0.5f),
  vec2(-0.5, -0.5f),
  vec2(0.5f, 0.5f)
);


/*

for(int i = 0; i < numPillars; ++i)
	{
		density += 1 / (length(frag_in.worldSpace.xy - pillars[i]) +1);
	}
	
	// shelve
	density += cos(height);
	
	// helix
	vec2 vec= vec2(cos(height), sin(height));
	density += dot(vec, frag_in.worldSpace.xy);
	
	// center hole
	density -= (1 / (length(frag_in.worldSpace.xy) +1)) * 0.3;
	
	// bounds. The further away from the centerer the more gets substracted
	density -=  pow( length(frag_in.worldSpace.xy), 3 );

	for(int i = 0; i < numPillars; ++i)
	{
		density +=  pillarFun(pillars[i]);
	}
*/


float calcPillarVal(vec2 pillar, float radius)
{
	float distanceToPillarCenter = length(frag_in.worldSpace.xy - pillar);	
	float normalisedDistance = distanceToPillarCenter / radius;	
	float result  = (1 / normalisedDistance) - 1;	
	return max(result,0);
}

float calcShelfVal(float frequency, float minVal, float maxVal)
{
	float normalizedVal = cos(frag_in.worldSpace.z * 2* pi * frequency);
	
	float result = (normalizedVal * (maxVal - minVal)) + minVal;
	return result;
}


void main(void) {
	
	float density = 0.2;
	
	
	// bounds. The further away from the centerer the more gets substracted
	density -=  pow( length(frag_in.worldSpace.xy), 2 );
	
	density += calcShelfVal(2, 0.6, 1);
	
	for(int i = 0; i < numPillars; ++i)
	{
		density += calcPillarVal(pillars[i], 0.2);
	}	
	
	// center hole
	density -= (1 / (length(frag_in.worldSpace.xy) +1));
	
	// bounds. The further away from the centerer the more gets substracted
	//density -=  pow( length(frag_in.worldSpace.xy), 3 );
	
	// close holes top / bottom negative density
	if(abs(0.5 - frag_in.worldSpace.z) > 0.49)
	{
		density -= 100;
	}
	
	finalDensity = density ;
	
}