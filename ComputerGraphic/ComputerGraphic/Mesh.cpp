#include "stdafx.h"
#include "Mesh.h"


void Mesh::Render()
{
	m_vertexArray.Bind();
	glDrawArrays(GL_TRIANGLES, 0, m_vertexCount);
	m_vertexArray.Unbind();
}
