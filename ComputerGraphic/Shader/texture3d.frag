#version 330 core
const float pi = 3.14;


out float finalDensity;

in VS_OUT
{
	vec3 worldSpace;
} frag_in;



uniform float baseDensity = 0;


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

float calcHelixVal(float frequency, float minVal, float maxVal)
{
	float scaledHight = frag_in.worldSpace.z * 2* pi * frequency;
	vec2 helixVec = vec2(cos(scaledHight), sin(scaledHight));
	float normalisedResult =  (dot(helixVec, frag_in.worldSpace.xy) +1) / 2;
	
	return (normalisedResult * (maxVal - minVal)) + minVal;
}


void main(void) {
	
	float density = baseDensity;
	
	
	// bounds. The further away from the centerer the more gets substracted
	density -=  pow( length(frag_in.worldSpace.xy), 2 );
	
	density += calcShelfVal(4, -0.3, 0.3);
	
	for(int i = 0; i < numPillars; ++i)
	{
		density += calcPillarVal(pillars[i], 0.2);
	}	
	
	density += calcHelixVal(2, 0, 1);
	
	// center hole
	density -= (1 / (length(frag_in.worldSpace.xy) +1));
	
	// bounds. The further away from the centerer the more gets substracted
	//density -=  pow( length(frag_in.worldSpace.xy), 4 );
	
	// close holes top / bottom negative density
	density -= max(abs(0.5 - frag_in.worldSpace.z) - 0.49,0) * 3000;
	
	// close hole on the sides
	density -= max(abs(frag_in.worldSpace.x) - 0.98,0) * 3000;
	density -= max(abs(frag_in.worldSpace.y) - 0.98,0) * 3000;


	finalDensity = density ;
	
}