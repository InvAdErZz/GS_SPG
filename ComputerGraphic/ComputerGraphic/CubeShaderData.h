#pragma once
struct CubeShaderData
{
	CubeShaderData() = default;
	CubeShaderData(glm::vec3 position, glm::vec4 color, int normalMapId)
		: position(position), color(color), normalMapId(normalMapId){}

	glm::vec3 position;
	glm::vec4 color;
	int normalMapId;
};