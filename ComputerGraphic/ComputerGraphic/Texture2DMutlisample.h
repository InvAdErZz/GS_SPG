#pragma once


#include "CheckGl.h"
#include <assert.h>
#include "Resource.h"
#include "vec2.hpp"
#include <vector>

class Texture2DMultisample : public Resource<Texture2DMultisample>
{
public:
	static std::vector<GLsizei> FindSupportedSampleSizes(glm::ivec2 resolution, GLsizei maxSampleSize = 128);
	static GLuint GetBoundHandle();

	bool IsBound() const;

	void Bind();
	void TexImage2DMultisample(GLsizei samples, GLenum internalFormat, glm::ivec2 size, bool hasFixedSampleLocations);

	void BindToTexture2DMultisampleUnit(GLuint Texture2DMultisampleUnit);

	static void Unbind();

	void Create();
	void FreeResource();
};

/////////////////////////////////////////////////////////////////////////
////////////////////////// Inline Functions /////////////////////////////
/////////////////////////////////////////////////////////////////////////

inline void Texture2DMultisample::Create()
{
	assert(!IsValid());
	glGenTextures(1, &m_handle);
	ASSERT_GL_ERROR_MACRO();
}

inline void Texture2DMultisample::FreeResource()
{
	ASSERT_GL_ERROR_MACRO();
	assert(IsValid());
	glDeleteTextures(1, &m_handle);
	ASSERT_GL_ERROR_MACRO();
}



inline GLuint Texture2DMultisample::GetBoundHandle()
{
	GLint result;
	glGetIntegerv(GL_TEXTURE_BINDING_2D_MULTISAMPLE, &result);
	GL_TEXTURE_BINDING_2D
	ASSERT_GL_ERROR_MACRO();
	return static_cast<GLuint>(result);
}

inline bool Texture2DMultisample::IsBound() const
{
	return m_handle == GetBoundHandle();
}

inline void Texture2DMultisample::Bind()
{
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_handle);
	ASSERT_GL_ERROR_MACRO();
}


inline void Texture2DMultisample::TexImage2DMultisample(GLsizei samples, GLenum internalFormat, glm::ivec2 size, bool hasFixedSampleLocations)
{
	assert(IsBound());
	glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat, size.x, size.y, hasFixedSampleLocations);
	ASSERT_GL_ERROR_MACRO();
}

inline void Texture2DMultisample::BindToTexture2DMultisampleUnit(GLuint textureUnit)
{
	assert(0 <= textureUnit && textureUnit <= (GL_TEXTURE31 - GL_TEXTURE0));

	glActiveTexture(textureUnit + GL_TEXTURE0);
	ASSERT_GL_ERROR_MACRO();
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_handle);
	ASSERT_GL_ERROR_MACRO();
}

inline void Texture2DMultisample::Unbind()
{
	glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	ASSERT_GL_ERROR_MACRO();
}

inline std::vector<GLsizei> Texture2DMultisample::FindSupportedSampleSizes(glm::ivec2 resolution, GLsizei maxSampleSize)
{
	Texture2DMultisample tester;
	tester.Create();
	tester.Bind();
	
	std::vector<GLsizei> result;
	result.reserve(8);

	glDisable(GL_DEBUG_OUTPUT);

	for (GLsizei i = 1; i < maxSampleSize; ++i)
	{
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, i, GL_RGB, resolution.x, resolution.y, false);
		GLenum error = glGetError();
		if (GL_NO_ERROR != error)
		{
			assert(GL_INVALID_OPERATION == error);
			continue;
		}

		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, i, GL_RGB, resolution.x, resolution.y, true);
		error = glGetError();
		if (GL_NO_ERROR != error)
		{
			assert(false);
			assert(GL_INVALID_OPERATION == error);
			continue;
		}

		result.push_back(i);
	}
	glEnable(GL_DEBUG_OUTPUT);

	ASSERT_GL_ERROR_MACRO();
	return result;	
}



