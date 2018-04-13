#pragma once
#include "glm.hpp"
#include <vector>

struct VertextAttribute
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 tangent;
	glm::vec3 bitangent;
	glm::vec2 texcoord;

	static constexpr GLuint POSITION_ATTRIBUTE_LOCATION = 0;
	static constexpr GLuint NORMAL_ATTRIBUTE_LOCATION = 1;
	static constexpr GLuint TANGENT_ATTRIBUTE_LOCATION = 2;
	static constexpr GLuint BITANGENT_ATTRIBUTE_LOCATION = 3;
	static constexpr GLuint TEXTCOORD_ATTRIBUTE_LOCATION = 4;
	


	static constexpr const char* POSITION_ATTRIBUTE_NAME = "in_Position";
	static constexpr const char* NORMAL_ATTRIBUTE_NAME = "in_Normal";
	static constexpr const char* TANGENT_ATTRIBUTE_NAME = "in_Tangent";
	static constexpr const char* BITANGENT_ATTRIBUTE_NAME = "in_Bitangent";
	static constexpr const char* TEXCOORD_ATTRIBUTE_NAME = "in_Texcoord";
};




struct MeshData
{
	std::vector<VertextAttribute> vertexData;
	std::vector<unsigned int> indexData;
};

namespace MeshCreation
{
	MeshData CreateRectangle_indexed(glm::vec3 length);
	MeshData CreateScreenTriangle_indexed();

	std::vector<std::vector<VertextAttribute>> LoadFromFile(const char* filename);
}