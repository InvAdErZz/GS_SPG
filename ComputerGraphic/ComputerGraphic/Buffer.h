#pragma once

#include "glew.h"
#include "Resource.h"
#include <vector>
#include <type_traits>
#include "BufferType.h"
#include "VertexArray.h"
#include "CheckGl.h"

template<BufferType TYPE>
class Buffer : public Resource<Buffer<TYPE>>
{
public:
	static constexpr int BUFFER_TARGET = GetBindBufferTarget(TYPE);
	static constexpr int BUFFER_BINDING = GetBufferBinding(TYPE);
	static GLuint GetBoundBufferHandle();

	bool IsBound() const;
	void Create();
	void Bind();
	static void Unbind();



	template<class ContainerType>
	void AllocateAndSetBufferData(const ContainerType& data);

	template<class DataType, int arraySize>
	void AllocateAndSetBufferData(DataType(&data)[arraySize]);

	template<class DataType> 
	void AllocateAndSetBufferData(const DataType* data, int dataSize);

	template<class DataType>
	void SetBufferData(const DataType* data, size_t numElementsToCopy, size_t writeOffsetInBuffer);

	void AllocateBufferData(GLsizeiptr size, GLenum usage);

	template<class DataType>
	void GetBufferData(DataType* outData, int numElementsToCopy, size_t readOffset);

	void FreeResource();
};

template<BufferType TYPE>
template<class DataType>
void Buffer<TYPE>::GetBufferData(DataType* outData, int numElementsToCopy, size_t readOffset)
{
	glGetBufferSubData(BUFFER_TARGET, readOffset * sizeof(DataType), numElementsToCopy * sizeof(DataType), static_cast<GLvoid*>(outData));
	ASSERT_GL_ERROR_MACRO();
}

template<BufferType TYPE>
template<class DataType>
void Buffer<TYPE>::SetBufferData(const DataType* data, size_t numElementsToCopy, size_t writeOffsetInBuffer)
{
	assert(IsBound());
	glBufferSubData(BUFFER_TARGET, writeOffsetInBuffer * sizeof(DataType), numElementsToCopy * sizeof(DataType), static_cast<const GLvoid*>(data));
	ASSERT_GL_ERROR_MACRO();
}

using IndexBuffer = Buffer<BufferType::Index>;


template<BufferType TYPE>
inline GLuint Buffer<TYPE>::GetBoundBufferHandle()
{
	GLint result;
	glGetIntegerv(BUFFER_BINDING, &result);
	ASSERT_GL_ERROR_MACRO();
	return static_cast<GLuint>(result);
}

template<BufferType TYPE>
inline void Buffer<TYPE>::Create()
{
	glGenBuffers(1, &m_handle);
	ASSERT_GL_ERROR_MACRO();
}

template<BufferType TYPE>
inline void Buffer<TYPE>::FreeResource()
{
	assert(IsValid());	
	glDeleteBuffers(1, &m_handle);	
	ASSERT_GL_ERROR_MACRO();
}

template<BufferType TYPE>
inline void Buffer<TYPE>::Bind()
{
	glBindBuffer(BUFFER_TARGET, m_handle);
	ASSERT_GL_ERROR_MACRO();
}

template<BufferType TYPE>
void Buffer<TYPE>::Unbind()
{
	glBindBuffer(BUFFER_TARGET, 0);
	ASSERT_GL_ERROR_MACRO();
}

template<BufferType TYPE>
inline bool Buffer<TYPE>::IsBound() const
{
	return IsValid() && GetBoundBufferHandle() == m_handle;
}

template<BufferType TYPE>
template<class DataType>
void Buffer<TYPE>::AllocateAndSetBufferData(const DataType* data, int dataSize)
{
	assert(IsBound());
	glBufferData(BUFFER_TARGET, sizeof(DataType) * dataSize, data, GL_STATIC_DRAW);
	ASSERT_GL_ERROR_MACRO();
}

template<BufferType TYPE>
template<class ContainerType>
void Buffer<TYPE>::AllocateAndSetBufferData(const ContainerType& data)
{
	assert(IsBound());
	glBufferData(BUFFER_TARGET, sizeof(ContainerType::value_type) * data.size(), data.data(), GL_STATIC_DRAW);
	ASSERT_GL_ERROR_MACRO();
}

template<BufferType TYPE>
template<class DataType, int arraySize>
void Buffer<TYPE>::AllocateAndSetBufferData(DataType(&data)[arraySize])
{
	assert(IsBound());
	glBufferData(BUFFER_TARGET, sizeof(DataType) * arraySize, data, GL_STATIC_DRAW);
	ASSERT_GL_ERROR_MACRO();
}


template<BufferType TYPE>
void Buffer<TYPE>::AllocateBufferData(GLsizeiptr size, GLenum usage)
{
	assert(IsBound());
	glBufferData(BUFFER_TARGET, size, nullptr, usage);
	ASSERT_GL_ERROR_MACRO();
}

