#version 330 core

const int maxLights = 2;

uniform sampler2D renderedscene;
out vec3 FragColor;
uniform sampler2D shadowmap[maxLights];
in vec2 texcoord;

void main(void) {
	FragColor = vec3(texture2D(renderedscene, texcoord).xyz);
	//float Depth = texture2D(shadowmap[1], texcoord).x;
	//FragColor = vec3(Depth,Depth,Depth);
	
	//FragColor.x = FragColor.x + texcoord.x;
	//FragColor.y = FragColor.y + texcoord.y;
}