#version 400

precision highp float;
 
const int maxLights = 2;

uniform bool isLightActive[maxLights];
uniform vec3 lightPos[maxLights];
uniform vec3 cameraPos;
uniform float ambient = 0.2;
uniform sampler2D colortexture;

in GEO_OUT
{
	vec3 wsPos;
	vec2 uv;
	vec3 normal;
} geo_in;

out vec3 color;

void main(void) {
	float totalLight = ambient;
	
	vec3 viewDir = normalize(cameraPos - geo_in.wsPos);
	vec3 normal = normalize(geo_in.normal);	
	
	for(int i = 0; i < maxLights; ++i)
	{
		vec3 lightDir = normalize(lightPos[0] - geo_in.wsPos);
		vec3 reflectDir = reflect(lightDir, normal);
		
		float diffuse = max(0,dot(normal, lightDir));
		float specular = max(0,dot(viewDir, reflectDir));
		specular = pow(specular, 20);
		
		float AllLightComponents = diffuse + specular;
		totalLight += AllLightComponents * 0.4;			
	}
	
	color = texture(colortexture, geo_in.uv).rgb * (totalLight);
}
