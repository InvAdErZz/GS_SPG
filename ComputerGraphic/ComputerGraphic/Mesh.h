#pragma once
#include "glew.h"
#include "glm.hpp"
#include <vector>

#include "Buffer.h"
#include "AttributeBuffer.h"
#include "VertexArray.h"
#include "MeshData.h"
#include "TypeToGLEnum.h"
class Mesh
{
public:
	void CreateInstanceOnGPU(const std::vector<VertextAttribute>& data);
	void Render();

private:
	VertexArray m_vertexArray;

	AttributeBuffer m_attribBuffer;
	GLint m_vertexCount;
};

inline void Mesh::CreateInstanceOnGPU(const std::vector<VertextAttribute>& data)
{
	m_vertexCount = data.size();

	m_vertexArray.Create();
	m_vertexArray.Bind();

	
	m_vertexArray.EnableAttribute(VertextAttribute::POSITION_ATTRIBUTE_LOCATION);
	m_vertexArray.EnableAttribute(VertextAttribute::NORMAL_ATTRIBUTE_LOCATION);
	m_vertexArray.EnableAttribute(VertextAttribute::TANGENT_ATTRIBUTE_LOCATION);
	m_vertexArray.EnableAttribute(VertextAttribute::BITANGENT_ATTRIBUTE_LOCATION);
	m_vertexArray.EnableAttribute(VertextAttribute::TEXTCOORD_ATTRIBUTE_LOCATION);	

	m_attribBuffer.Create();
	m_attribBuffer.Bind();
	m_attribBuffer.UploadBufferData(data);

	m_attribBuffer.SetVertexAttributePtr(
		VertextAttribute::POSITION_ATTRIBUTE_LOCATION,
		decltype(VertextAttribute::position)::length(),
		TypeToGLEnum<decltype(VertextAttribute::position)::value_type>::value,
		sizeof(VertextAttribute),
		offsetof(VertextAttribute, position)
		);

	m_attribBuffer.SetVertexAttributePtr(
		VertextAttribute::NORMAL_ATTRIBUTE_LOCATION,
		decltype(VertextAttribute::normal)::length(),
		TypeToGLEnum<decltype(VertextAttribute::normal)::value_type>::value,
		sizeof(VertextAttribute),
		offsetof(VertextAttribute, normal)
	);

	m_attribBuffer.SetVertexAttributePtr(
		VertextAttribute::TANGENT_ATTRIBUTE_LOCATION,
		decltype(VertextAttribute::tangent)::length(),
		TypeToGLEnum<decltype(VertextAttribute::tangent)::value_type>::value,
		sizeof(VertextAttribute),
		offsetof(VertextAttribute, tangent)
	);

	m_attribBuffer.SetVertexAttributePtr(
		VertextAttribute::BITANGENT_ATTRIBUTE_LOCATION,
		decltype(VertextAttribute::bitangent)::length(),
		TypeToGLEnum<decltype(VertextAttribute::bitangent)::value_type>::value,
		sizeof(VertextAttribute),
		offsetof(VertextAttribute, bitangent)
	);

	m_attribBuffer.SetVertexAttributePtr(
		VertextAttribute::TEXTCOORD_ATTRIBUTE_LOCATION,
		decltype(VertextAttribute::texcoord)::length(),
		TypeToGLEnum<decltype(VertextAttribute::texcoord)::value_type>::value,
		sizeof(VertextAttribute),
		offsetof(VertextAttribute, texcoord)
	);
	
	m_attribBuffer.Unbind();
}
