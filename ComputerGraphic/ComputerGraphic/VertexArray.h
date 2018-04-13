#pragma once
#include "glew.h"
#include "Resource.h"
#include <assert.h>
#include "CheckGl.h"


class VertexArray : public Resource<VertexArray>
{
public:
	static GLuint GetBoundHandle();

	bool IsBound() const;
	void Create();
	void Bind();
	static void Unbind();
	void EnableAttribute(int index);
	void FreeResource();
};

inline void VertexArray::FreeResource()
{
	assert(IsValid());
	glDeleteVertexArrays(1, &m_handle);
	ASSERT_GL_ERROR_MACRO();
}

inline GLuint VertexArray::GetBoundHandle()
{
	GLint result;
	glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &result);
	ASSERT_GL_ERROR_MACRO();
	return static_cast<GLuint>(result);
}

inline bool VertexArray::IsBound() const
{
	return IsValid() &&  m_handle == GetBoundHandle();
}

inline void VertexArray::Create()
{
	assert(!IsValid());
	glGenVertexArrays(1, &m_handle);
	ASSERT_GL_ERROR_MACRO();
}

inline void VertexArray::Bind()
{
	glBindVertexArray(m_handle);
	ASSERT_GL_ERROR_MACRO();
}

inline void VertexArray::Unbind()
{
	glBindVertexArray(0);
	ASSERT_GL_ERROR_MACRO();
}

inline void VertexArray::EnableAttribute(int index)
{
	assert(IsBound());
	glEnableVertexAttribArray(index);
	ASSERT_GL_ERROR_MACRO();
}