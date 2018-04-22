#version 330

precision highp float;
 
const int maxLights = 2;
 
out vec3 color;

void main(void) {
	
	vec3 ex_Color = vec3(1,0,0);	
	color = ex_Color.xyz;
}
