#include "stdafx.h"
#include "MeshIndexed.h"


void MeshIndexed::CreateInstanceOnGPU(const MeshData & data)
{
	m_indexCount = data.indexData.size();
	m_vertexArray.Create();
	m_vertexArray.Bind();
	m_vertexArray.EnableAttribute(VertextAttribute::POSITION_ATTRIBUTE_LOCATION);

	m_indexBuffer.Create();
	m_indexBuffer.Bind();
	m_indexBuffer.UploadBufferData(data.indexData);
	m_indexBuffer.Unbind();

	m_attribBuffer.Create();
	m_attribBuffer.Bind();
	m_attribBuffer.UploadBufferData(data.vertexData);
	m_attribBuffer.SetVertexAttributePtr(VertextAttribute::POSITION_ATTRIBUTE_LOCATION, 3, GL_FLOAT, sizeof(VertextAttribute), offsetof(VertextAttribute, position));
	m_attribBuffer.Unbind();
}

void MeshIndexed::Render()
{
	m_vertexArray.Bind();
	m_indexBuffer.Bind();
	glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);
	m_indexBuffer.Unbind();
	m_vertexArray.Unbind();
}
