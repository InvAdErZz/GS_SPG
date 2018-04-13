#pragma once

#include "glew.h"
#include "glm.hpp"
#include <vector>

#include "Buffer.h"
#include "AttributeBuffer.h"
#include "VertexArray.h"
#include "MeshData.h"



class MeshIndexed
{
public:
	void CreateInstanceOnGPU(const MeshData& data);
	void Render();


private:
	VertexArray m_vertexArray;

	IndexBuffer m_indexBuffer;
	AttributeBuffer m_attribBuffer;

	GLuint m_indexCount = 0;
};
