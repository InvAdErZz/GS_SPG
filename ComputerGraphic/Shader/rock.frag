#version 330

precision highp float;
 
const int maxLights = 2;

uniform vec3 cameraPos;
uniform vec3 lightPos[maxLights];
 
in VS_OUT
{
	vec3 wsPosition;
	vec3 wsNormal;
} frag_in;
 
out vec3 color;

void main(void) {
	
	float totalLight = 0.1f;
	
	vec3 norm = normalize(frag_in.wsNormal);
    vec3 lightDir = normalize(lightPos[0] - frag_in.wsPosition);
    float diff = max(dot(norm, lightDir), 0.0);
	totalLight += diff;
	
	vec3 viewDir = normalize(cameraPos - frag_in.wsNormal);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    totalLight += spec;
	
	
	vec3 ex_Color = vec3(1,1,1);
	color = ex_Color.xyz * totalLight;
	//color = normalize(vec3(1,1,1) + norm);
}
