#pragma once


#include "CheckGl.h"
#include <assert.h>
#include "Resource.h"
class Texture : public Resource<Texture>
{
public:
	static GLuint GetBoundHandle();

	bool IsBound() const;
	
	void Bind();
	void TextureImage(GLint levelOfDetail, GLint internalFormat, GLsizei width, GLsizei heigth, GLenum format, GLenum pixelDataType, const void* imageData);
	void SetNearestNeighbourFiltering();
	void SetLinearFiltering();
	void SetClampToEdge();

	void SetClampBorder(glm::vec4 borderColor);
	void SetRepeating();
	void BindToTextureUnit(GLuint textureUnit);

	static void Unbind();

	void Create();
	void FreeResource();
};

  /////////////////////////////////////////////////////////////////////////
 ////////////////////////// Inline Functions /////////////////////////////
/////////////////////////////////////////////////////////////////////////

inline void Texture::Create()
{
	assert(!IsValid());
	glGenTextures(1, &m_handle);
	ASSERT_GL_ERROR_MACRO();
}

inline void Texture::FreeResource()
{
	ASSERT_GL_ERROR_MACRO();
	glDeleteTextures(1, &m_handle);
	ASSERT_GL_ERROR_MACRO();
}

inline GLuint Texture::GetBoundHandle()
{
	GLint result;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &result);
	ASSERT_GL_ERROR_MACRO();
	return static_cast<GLuint>(result);
}


inline bool Texture::IsBound() const
{
	return m_handle == GetBoundHandle();
}

inline void Texture::Bind()
{
	glBindTexture(GL_TEXTURE_2D, m_handle);
	ASSERT_GL_ERROR_MACRO();
}

inline void Texture::TextureImage(GLint levelOfDetail, GLint internalFormat, GLsizei width, GLsizei heigth, GLenum format, GLenum pixelDataType, const void* imageData)
{
	assert(IsBound());
	glTexImage2D(GL_TEXTURE_2D, levelOfDetail, internalFormat, width, heigth, 0, format, pixelDataType, imageData);
	ASSERT_GL_ERROR_MACRO();
}

inline void Texture::SetNearestNeighbourFiltering()
{
	assert(IsBound());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	ASSERT_GL_ERROR_MACRO();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	ASSERT_GL_ERROR_MACRO();

}

inline void Texture::SetLinearFiltering()
{
	assert(IsBound());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	ASSERT_GL_ERROR_MACRO();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	ASSERT_GL_ERROR_MACRO();

}

inline void Texture::SetClampToEdge()
{
	assert(IsBound());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	ASSERT_GL_ERROR_MACRO();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	ASSERT_GL_ERROR_MACRO();
}

inline void Texture::SetClampBorder(glm::vec4 borderColor)
{
	assert(IsBound());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	ASSERT_GL_ERROR_MACRO();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	ASSERT_GL_ERROR_MACRO();
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(borderColor));
	ASSERT_GL_ERROR_MACRO();		
}

inline void Texture::SetRepeating()
{
	assert(IsBound());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	ASSERT_GL_ERROR_MACRO();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	ASSERT_GL_ERROR_MACRO();
}


inline void Texture::BindToTextureUnit(GLuint textureUnit)
{
	assert(0 <= textureUnit && textureUnit <= (GL_TEXTURE31 - GL_TEXTURE0));

	glActiveTexture(textureUnit + GL_TEXTURE0);
	ASSERT_GL_ERROR_MACRO();
	glBindTexture(GL_TEXTURE_2D, m_handle);
	ASSERT_GL_ERROR_MACRO();
}

inline void Texture::Unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
	ASSERT_GL_ERROR_MACRO();
}


