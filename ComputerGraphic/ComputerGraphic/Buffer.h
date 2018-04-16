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

	template<class DataType>
	void UploadBufferData(const std::vector<DataType>& data);

	template<class DataType, int arraySize>
	void UploadBufferData(DataType(&data)[arraySize]);

	void AllocateBufferData(GLsizeiptr size, GLenum usage);


	void FreeResource();
};

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
void Buffer<TYPE>::UploadBufferData(const std::vector<DataType>& data)
{
	assert(IsBound());
	glBufferData(BUFFER_TARGET, sizeof(DataType) * data.size(), data.data(), GL_STATIC_DRAW);
	ASSERT_GL_ERROR_MACRO();
}

template<BufferType TYPE>
template<class DataType, int arraySize>
void Buffer<TYPE>::UploadBufferData(DataType(&data)[arraySize])
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

