#version 330

precision highp float;
 
const int maxLights = 2;

uniform vec3 cameraPos;
uniform vec3 lightPos[maxLights];

uniform sampler2D colorTex;
uniform sampler2D normalTex;
uniform sampler2D dispTex;
 
in VS_OUT
{
	vec3 wsPosition;
	vec3 wsNormal;
	vec3 uv;
} frag_in;
 
out vec3 color;

vec3 UnpackNormal(vec3 textureNormal)
{
	vec3 normal = normalize((textureNormal * 2) - 1);
	return normal;
}

vec2 Paralax(vec3 viewTS, vec2 uv)
{
	viewTS = normalize(viewTS);
	int numLayers = 5;
	int numRefinementLayers = 5;
	float layerStep = 1.f / numLayers;
	float refinementLayerStep = layerStep / numRefinementLayers;

	float heightScale = 0.01f;
	//heightScale = 0.025f;
	vec2 maxOffset = (viewTS.xy / viewTS.z) * heightScale;
	vec2 offsetPerLayer = maxOffset * layerStep;
	vec2 offsetPerRefinementLayer = maxOffset * refinementLayerStep;

	vec2 currentTexCoords = uv;
	float currentLayerZ = 1.0f;
	float currentDispZ = texture(dispTex, currentTexCoords).r;
	
	float displaceAmount = 1- currentDispZ;
	//return uv + maxOffset * displaceAmount;
	
	for(int i = 0; i < numLayers; ++i)
	{
		if(currentLayerZ > currentDispZ)
		{
			currentTexCoords += offsetPerLayer;
			currentLayerZ -= layerStep;
		}
		// always fetch to avoid non uniform control flow texture lookup
		float textureVal = texture(dispTex, currentTexCoords).r;
		
		if(currentLayerZ > currentDispZ)
		{
			currentDispZ = textureVal;
		}
	}
	
	return currentTexCoords - offsetPerLayer/2;
	
#if 0
	// now we have our layer is lower than our current Depth
	// we will now reverse back wiht refinement steps until this is no longer true	
	for(int i = 0; i < numRefinementLayers; ++i)
	{
		if(currentLayerZ < currentDispZ)
		{
			currentTexCoords -= offsetPerLayer;
			currentLayerZ += layerStep;
		}
		
		// always fetch to avoid non uniform control flow texture lookup
		float textureVal = texture(dispTex, currentTexCoords).r;
		
		if(currentLayerZ < currentDispZ)
		{
			currentDispZ = textureVal;
		}
	}
#endif
	
	return currentTexCoords;
}


void main(void) {
	float powValue = 3;
	vec3 powVec = vec3(powValue,powValue,powValue);
	vec3 normalizedWsNormal = normalize(frag_in.wsNormal);
	vec3 viewDir = normalize(cameraPos - frag_in.wsPosition);
	
	vec3 blendfactor = pow(abs(normalizedWsNormal), powVec) - 0.1;
	blendfactor = max(vec3(0), blendfactor);
	//vec3 blendfactor = pow(abs(normalizedWsNormal) -, powVec);

	blendfactor /= dot(blendfactor, vec3(1));

	vec2 uvX = frag_in.uv.yz;
	vec2 uvY = frag_in.uv.xz;
	vec2 uvZ = frag_in.uv.xy;
	
	vec3 viewAxisSign = sign(viewDir);
	
	vec3 viewTsX;
	vec3 viewTsY;
	vec3 viewTsZ;
	
	viewTsX = viewDir.zyx;
	viewTsY = viewDir.xzy;	
	viewTsZ = viewDir.xyz;
	
#if 0
	viewTsX.z *= viewAxisSign.x;
	viewTsY.z *= viewAxisSign.y;
	viewTsZ.z *= viewAxisSign.z;
#endif 

#if 1
	uvX = Paralax(viewTsX, uvX);
	
	uvY = Paralax(viewTsY, uvY);
	uvZ = Paralax(viewTsZ, uvZ);
#endif
	// normals in tangent space
	vec3 tsNormX = UnpackNormal(texture(normalTex, uvX).xyz);
	vec3 tsNormY = UnpackNormal(texture(normalTex, uvY).xyz);
	vec3 tsNormZ = UnpackNormal(texture(normalTex, uvZ).xyz);


	// perform UDN blend. For the most significant direction the only original normals value is used
	// for the other directions perfom a blend
	// ws normal is brought into tangent space via swizzles	
	tsNormX = vec3(tsNormX.xy + normalizedWsNormal.yz, normalizedWsNormal.x);
	tsNormY = vec3(tsNormY.xy + normalizedWsNormal.xz, normalizedWsNormal.y);
	tsNormZ = vec3(tsNormZ.xy + normalizedWsNormal.xy, normalizedWsNormal.z);

	// Convert ts to worldspace via swizzels and perform triplanar blending
	vec3 finalNormal = normalize(
		tsNormX.zxy * blendfactor.x +
		tsNormY.xzy * blendfactor.y +
		tsNormZ.xyz * blendfactor.z
	);
	
	float totalLight = 0.2f;

    vec3 lightDir = normalize(lightPos[0] - frag_in.wsPosition);
    float diff = max(dot(finalNormal, lightDir), 0.0);
	totalLight += diff;
	
	
    vec3 reflectDir = reflect(-lightDir, finalNormal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    totalLight += spec;
	
	
	vec3 ex_Color =
		texture(colorTex, uvX).xyz * blendfactor.x +
		texture(colorTex, uvY).xyz * blendfactor.y +
		texture(colorTex, uvZ).xyz * blendfactor.z 
		//+ blendfactor * 0.3
		;


	color = ex_Color.xyz * totalLight;
}
