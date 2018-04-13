#pragma once

#include "CheckGl.h"
#include <assert.h>
#include "Resource.h"
#include "vec2.hpp"
class Renderbuffer : public Resource<Renderbuffer>
{
public:
	static GLuint GetBoundHandle();

	bool IsBound() const;

	void Bind();
	void Storage(GLenum internalFormat, glm::ivec2 size);
	void StorageMultisample(GLsizei samples, GLenum internalFormat, glm::ivec2 size);

	static void Unbind();

	void Create();
	void FreeResource();
};

/////////////////////////////////////////////////////////////////////////
////////////////////////// Inline Functions /////////////////////////////
/////////////////////////////////////////////////////////////////////////

inline void Renderbuffer::Create()
{
	assert(!IsValid());
	glGenRenderbuffers(1, &m_handle);

	ASSERT_GL_ERROR_MACRO();
}

inline void Renderbuffer::FreeResource()
{
	ASSERT_GL_ERROR_MACRO();
	glDeleteRenderbuffers(1, &m_handle);
	ASSERT_GL_ERROR_MACRO();
}

inline GLuint Renderbuffer::GetBoundHandle()
{
	GLint result;
	glGetIntegerv(GL_RENDERBUFFER_BINDING, &result);
	ASSERT_GL_ERROR_MACRO();
	return static_cast<GLuint>(result);
}


inline bool Renderbuffer::IsBound() const
{
	return m_handle == GetBoundHandle();
}

inline void Renderbuffer::Bind()
{
	glBindRenderbuffer(GL_RENDERBUFFER, m_handle);
	ASSERT_GL_ERROR_MACRO();
	assert(IsBound());
}

inline void Renderbuffer::Storage(GLenum internalFormat, glm::ivec2 size)
{
	assert(IsBound());
	glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, size.x, size.y);
	ASSERT_GL_ERROR_MACRO();
}

inline void Renderbuffer::StorageMultisample(GLsizei samples, GLenum internalFormat, glm::ivec2 size)
{
	assert(IsBound());
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, internalFormat, size.x, size.y);
	ASSERT_GL_ERROR_MACRO();
}


inline void Renderbuffer::Unbind()
{
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	ASSERT_GL_ERROR_MACRO();
}


