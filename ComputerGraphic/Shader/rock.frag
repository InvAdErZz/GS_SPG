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

vec2 Parallax2(sampler2D map, vec2 texCoords, vec3 viewDir)
{
	int displacement_initialSteps = 8;
	int displacement_refinementSteps = 8;
	float displacement_scale = 0.025f;
	displacement_scale = 1.f;
    // calculate the size of each layer
    float layerDepth = 1.0 / displacement_initialSteps;
    // depth of current layer
    float currentLayerDepth = 0.0;
    // the amount to shift the texture coordinates per layer (from vector P)
    vec2 P = vec2(viewDir.x, -viewDir.y) / viewDir.z * displacement_scale;
    vec2 deltaTexCoords = P / displacement_initialSteps;

    // get initial values
    vec2  currentTexCoords     = texCoords;
    float currentDepthMapValue = texture(map, currentTexCoords).r;

 	while(currentLayerDepth < currentDepthMapValue)
    {
        // shift texture coordinates along direction of P
        currentTexCoords -= deltaTexCoords;
        // get depthmap value at current texture coordinates
        currentDepthMapValue = texture(map, currentTexCoords).r;
        // get depth of next layer
        currentLayerDepth += layerDepth;
    }

    currentTexCoords += deltaTexCoords;
    currentDepthMapValue = texture(map, currentTexCoords).r;
    currentLayerDepth -= layerDepth;
	currentLayerDepth -= 0.085f; //reduces artifacts

	// decrease the step size as we do the refinement steps
	deltaTexCoords /= displacement_refinementSteps;
	layerDepth /= displacement_refinementSteps;

 	while(currentLayerDepth < currentDepthMapValue)
    {
        // shift texture coordinates along direction of P
        currentTexCoords -= deltaTexCoords;
        // get depthmap value at current texture coordinates
        currentDepthMapValue = texture(map, currentTexCoords).r;
        // get depth of next layer
        currentLayerDepth += layerDepth;
    }

    // get texture coordinates before collision (reverse operations)
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;
	float prevLayerDepth = currentLayerDepth - layerDepth;

    // get depth after and before collision for linear interpolation
    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(map, prevTexCoords).r - prevLayerDepth;

    // interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;
}

vec2 Paralax(vec3 viewTS, vec2 uv)
{
	viewTS = normalize(viewTS);
	int numLayers = 5;
	int numRefinementLayers = 5;
	float layerStep = 1.f / numLayers;
	float refinementLayerStep = layerStep / numRefinementLayers;

	float heightScale = 10.1f;
	heightScale = 0.025f;
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
	float powValue = 5;
	vec3 powVec = vec3(powValue,powValue,powValue);
	vec3 normalizedWsNormal = normalize(frag_in.wsNormal);
	vec3 viewDir = normalize(cameraPos - normalizedWsNormal);
	
	vec3 blendfactor = pow(abs(normalizedWsNormal), powVec);

	blendfactor /= (blendfactor.x + blendfactor.y + blendfactor.z);

	vec2 uvX = frag_in.uv.yz;
	vec2 uvY = frag_in.uv.xz;
	vec2 uvZ = frag_in.uv.xy;
	
	vec3 viewAxisSign = sign(viewDir);
	
	vec3 viewTsX;
	vec3 viewTsY;
	vec3 viewTsZ;
	
	viewTsX = viewDir.yzx;
	viewTsY = viewDir.xzy;	
	viewTsZ = viewDir.xyz;
	
#if 0
	vec3 viewTsX.zxy = viewDir.zyx;
	vec3 viewTsY.xzy = viewDir.xzy;	
	vec3 viewTsZ.xyz = viewDir.xyz;
#endif
#if 1
	viewTsX.z *= viewAxisSign.x;
	viewTsY.z *= viewAxisSign.y;
	viewTsZ.z *= viewAxisSign.z;
#endif 
#if 0
	uvX = Parallax2(dispTex, uvX, viewTsX);
	uvY = Parallax2(dispTex, uvY, viewTsY);
	uvZ = Parallax2(dispTex, uvZ, viewTsZ);
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
	tsNormX = vec3(tsNormX.xy + normalizedWsNormal.zy, normalizedWsNormal.x);
	tsNormY = vec3(tsNormX.xy + normalizedWsNormal.xz, normalizedWsNormal.y);
	tsNormZ = vec3(tsNormX.xy + normalizedWsNormal.xy, normalizedWsNormal.z);

	// Convert ts to worldspace via swizzels and perform triplanar blending
	vec3 finalNormal = normalize(
		tsNormX.zxy * blendfactor.x +
		tsNormY.xzy * blendfactor.y +
		tsNormZ.xyz * blendfactor.z
	);
	
	float totalLight = 0.1f;

    vec3 lightDir = normalize(lightPos[0] - frag_in.wsPosition);
    float diff = max(dot(finalNormal, lightDir), 0.0);
	totalLight += diff;
	
	
    vec3 reflectDir = reflect(-lightDir, finalNormal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    totalLight += spec;
	
	
	vec3 ex_Color =
		texture(colorTex, uvX).xyz * blendfactor.x +
		texture(colorTex, uvY).xyz * blendfactor.y +
		texture(colorTex, uvZ).xyz * blendfactor.z ;


	color = ex_Color.xyz * totalLight;
}
