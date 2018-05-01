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


void main(void) {
	float powValue = 5;
	vec3 powVec = vec3(powValue,powValue,powValue);
	vec3 normalizedWsNormal = normalize(frag_in.wsNormal);

	vec3 blendfactor = pow(abs(normalizedWsNormal), powVec);

	blendfactor /= (blendfactor.x + blendfactor.y + blendfactor.z);

	vec2 uvX = frag_in.uv.yz;
	vec2 uvY = frag_in.uv.xz;
	vec2 uvZ = frag_in.uv.xy;

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
	
	vec3 viewDir = normalize(cameraPos - normalizedWsNormal);
    vec3 reflectDir = reflect(-lightDir, finalNormal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    totalLight += spec;
	
	
	vec3 ex_Color =
		texture(colorTex, uvX).xyz * blendfactor.x +
		texture(colorTex, uvY).xyz * blendfactor.y +
		texture(colorTex, uvZ).xyz * blendfactor.z ;


	color = ex_Color.xyz * totalLight;
}
