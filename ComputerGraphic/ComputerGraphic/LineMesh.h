#pragma once

#include <array>
#include "Buffer.h"
#include "AttributeBuffer.h"
#include "VertexArray.h"
#include "MeshData.h"
#include "TypeToGLEnum.h"
#include "vec3.hpp"

class LineMesh
{

	public:
		void CreateInstanceOnGPU(const std::vector<glm::vec3>& data);
		void Render();

	private:
		VertexArray m_vertexArray;

		AttributeBuffer m_attribBuffer;
		GLint m_vertexCount;

};