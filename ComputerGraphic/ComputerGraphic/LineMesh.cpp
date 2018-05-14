#include "stdafx.h"
#include "LineMesh.h"

void LineMesh::CreateInstanceOnGPU(const std::vector<glm::vec3>& data)
{
	m_vertexCount = data.size();

	m_vertexArray.Create();
	m_vertexArray.Bind();


	m_vertexArray.EnableAttribute(0);
	
	m_attribBuffer.Create();
	m_attribBuffer.Bind();
	m_attribBuffer.AllocateAndSetBufferData(data);

	//m_attribBuffer.SetVertexAttributePtr(
	//	0,
	//	LineData::value_type::length(),
	//	TypeToGLEnum<LineData::value_type::value_type>::value,
	//	sizeof(LineData),
	//	0
	//);

	m_attribBuffer.SetVertexAttributePtr(
		0,
		3,
		GL_FLOAT,
		sizeof(glm::vec3),
		0
	);

	m_attribBuffer.Unbind();
}

void LineMesh::Render()
{
	m_vertexArray.Bind();
	glDrawArrays(GL_LINES, 0, m_vertexCount);
	m_vertexArray.Unbind();
}
