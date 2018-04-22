#pragma once

#include "Buffer.h"
class AttributeBuffer : public Buffer<BufferType::Attribute>
{
public:
	void SetVertexAttributePtr(int index, int numElements, GLenum Datatype, int VertexDataSize, size_t offset);
};

inline void AttributeBuffer::SetVertexAttributePtr(int index, int numElements, GLenum elementType, int VertexDataSize, size_t offset)
{
	assert(IsBound());
	assert(VertexArray::GetBoundHandle() != 0);

	glEnableVertexAttribArray(index);
	ASSERT_GL_ERROR_MACRO();

	glVertexAttribPointer(index, numElements, elementType, false, VertexDataSize, reinterpret_cast<GLvoid const*>(offset));
	ASSERT_GL_ERROR_MACRO();
}