#pragma once
#include "glew.h"
#include "Resource.h"
#include <assert.h>
#include "CheckGl.h"


class FrameBuffer : public Resource<FrameBuffer>
{
public:
	static GLuint GetBoundHandle();
	static FrameBuffer DefaultFrameBuffer();

	bool IsBound() const;
	void Create();
	void Bind();

	void BindAsReadBuffer();
	void BindAsDrawBuffer();

	static void UnbindAsReadBuffer();
	static void UnbindAsDrawBuffer();
	static void Unbind();

	void FreeResource();
	void BindTexture(GLenum attachment, GLuint textureHandle, GLint mipMapLevel);
	void BindMultisampleTexture(GLenum attachment, GLuint textureHandle);
	void BindRenderbuffer(GLenum attachment, GLuint renderbufferHandle);
	void SetDrawBuffers(GLenum buffer);
	void SetReadBuffers(GLenum buffer);
	GLenum GetFrameBufferStatus() const;
};


inline void FrameBuffer::FreeResource()
{
	assert(IsValid());
	if (m_handle != 0)
	{
		glDeleteFramebuffers(1, &m_handle);
	}
	ASSERT_GL_ERROR_MACRO();
}

inline void FrameBuffer::BindTexture(GLenum attachment, GLuint textureHandle, GLint mipMapLevel)
{
	assert(IsBound());
	glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, textureHandle, mipMapLevel);
	ASSERT_GL_ERROR_MACRO();
}

inline void FrameBuffer::BindMultisampleTexture(GLenum attachment, GLuint textureHandle)
{
	assert(IsBound());
	glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D_MULTISAMPLE, textureHandle,0);
	ASSERT_GL_ERROR_MACRO();
}

inline void FrameBuffer::BindRenderbuffer(GLenum attachment, GLuint renderbufferHandle)
{
	assert(IsBound());
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, renderbufferHandle);
	ASSERT_GL_ERROR_MACRO();
}

inline void FrameBuffer::SetDrawBuffers(GLenum buffer)
{
	assert(IsBound());
	glDrawBuffer(buffer);
	ASSERT_GL_ERROR_MACRO();
}

inline void FrameBuffer::SetReadBuffers(GLenum buffer)
{
	assert(IsBound());
	glReadBuffer(buffer);
	ASSERT_GL_ERROR_MACRO();
}

inline GLenum FrameBuffer::GetFrameBufferStatus() const
{
	assert(IsBound());
	const GLenum result =  glCheckFramebufferStatus(GL_FRAMEBUFFER);
	ASSERT_GL_ERROR_MACRO();
	return result;
}

inline GLuint FrameBuffer::GetBoundHandle()
{
	GLint result;
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &result);
	ASSERT_GL_ERROR_MACRO();
	return static_cast<GLuint>(result);
}

inline FrameBuffer FrameBuffer::DefaultFrameBuffer()
{
	FrameBuffer defaultBuffer;
	defaultBuffer.m_handle = 0;
	return defaultBuffer;
}

inline bool FrameBuffer::IsBound() const
{
	return IsValid() && m_handle == GetBoundHandle();
}

inline void FrameBuffer::Create()
{
	assert(!IsValid());
	glGenFramebuffers(1, &m_handle);
	ASSERT_GL_ERROR_MACRO();
}

inline void FrameBuffer::Bind()
{
	assert(IsValid());
	glBindFramebuffer(GL_FRAMEBUFFER, m_handle);
	ASSERT_GL_ERROR_MACRO();
}

inline void FrameBuffer::BindAsReadBuffer()
{
	assert(IsValid());
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_handle);
	ASSERT_GL_ERROR_MACRO();
}

inline void FrameBuffer::BindAsDrawBuffer()
{
	assert(IsValid());
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_handle);
	ASSERT_GL_ERROR_MACRO();
}

inline void FrameBuffer::UnbindAsReadBuffer()
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	ASSERT_GL_ERROR_MACRO();
}

inline void FrameBuffer::UnbindAsDrawBuffer()
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	ASSERT_GL_ERROR_MACRO();
}

inline void FrameBuffer::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	ASSERT_GL_ERROR_MACRO();
}

namespace FrameBufferUtils
{
	inline void CopyBufferData(FrameBuffer& source, FrameBuffer& destination, glm::ivec2 size)
	{
		auto currentBoundFramebuffer = FrameBuffer::GetBoundHandle();
	
		assert((source.Bind(), GL_FRAMEBUFFER_COMPLETE == source.GetFrameBufferStatus()));
		assert((destination.Bind(), GL_FRAMEBUFFER_COMPLETE == destination.GetFrameBufferStatus()));
	
		source.BindAsReadBuffer();
		destination.BindAsDrawBuffer();
		glBlitFramebuffer(0, 0, size.x, size.y, 0, 0, size.x, size.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);
		ASSERT_GL_ERROR_MACRO();
		glBindFramebuffer(GL_FRAMEBUFFER, currentBoundFramebuffer);	
		ASSERT_GL_ERROR_MACRO();
	}
}
