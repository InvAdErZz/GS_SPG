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

	enum Location
	{
		POSITION_ATTRIBUTE_LOCATION = 0,
		NORMAL_ATTRIBUTE_LOCATION,
		TANGENT_ATTRIBUTE_LOCATION,
		BITANGENT_ATTRIBUTE_LOCATION,
		TEXTCOORD_ATTRIBUTE_LOCATION,
		enum_size
	};


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