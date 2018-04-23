#pragma once


#include "CheckGl.h"
#include <assert.h>
#include "Resource.h"
#include <glm.hpp>
#include <gtc/type_ptr.hpp>
class Texture3d : public Resource<Texture3d>
{
public:
	static GLuint GetBoundHandle();

	bool IsBound() const;

	void Bind();
	void TextureImage(GLint levelOfDetail, GLint internalFormat, GLsizei width, GLsizei heigth, GLsizei depth, GLenum format, GLenum pixelDataType, const void* imageData);
	void SetNearestNeighbourFiltering();
	void SetLinearFiltering();
	void SetClampToEdge();

	void SetClampBorder(glm::vec4 borderColor);
	void BindToTextureUnit(GLuint textureUnit);

	static void Unbind();

	void Create();
	void FreeResource();
};

/////////////////////////////////////////////////////////////////////////
////////////////////////// Inline Functions /////////////////////////////
/////////////////////////////////////////////////////////////////////////

inline void Texture3d::Create()
{
	assert(!IsValid());
	glGenTextures(1, &m_handle);
	ASSERT_GL_ERROR_MACRO();
}

inline void Texture3d::FreeResource()
{
	ASSERT_GL_ERROR_MACRO();
	glDeleteTextures(1, &m_handle);
	ASSERT_GL_ERROR_MACRO();
}

inline GLuint Texture3d::GetBoundHandle()
{
	GLint result;
	glGetIntegerv(GL_TEXTURE_BINDING_3D, &result);
	ASSERT_GL_ERROR_MACRO();
	return static_cast<GLuint>(result);
}


inline bool Texture3d::IsBound() const
{
	return m_handle == GetBoundHandle();
}

inline void Texture3d::Bind()
{
	glBindTexture(GL_TEXTURE_3D, m_handle);
	ASSERT_GL_ERROR_MACRO();
}

inline void Texture3d::TextureImage(GLint levelOfDetail, GLint internalFormat, GLsizei width, GLsizei heigth,GLsizei depth, GLenum format, GLenum pixelDataType, const void* imageData)
{
	assert(IsBound());
	glTexImage3D(GL_TEXTURE_3D, levelOfDetail, internalFormat, width, heigth, depth, 0, format, pixelDataType, imageData);
	ASSERT_GL_ERROR_MACRO();
}

inline void Texture3d::SetNearestNeighbourFiltering()
{
	assert(IsBound());
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	ASSERT_GL_ERROR_MACRO();
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	ASSERT_GL_ERROR_MACRO();

}

inline void Texture3d::SetLinearFiltering()
{
	assert(IsBound());
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	ASSERT_GL_ERROR_MACRO();
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	ASSERT_GL_ERROR_MACRO();

}

inline void Texture3d::SetClampToEdge()
{
	assert(IsBound());
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	ASSERT_GL_ERROR_MACRO();
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	ASSERT_GL_ERROR_MACRO();
}

inline void Texture3d::SetClampBorder(glm::vec4 borderColor)
{
	assert(IsBound());
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	ASSERT_GL_ERROR_MACRO();
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	ASSERT_GL_ERROR_MACRO();
	glTexParameterfv(GL_TEXTURE_3D, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(borderColor));
	ASSERT_GL_ERROR_MACRO();
}

inline void Texture3d::BindToTextureUnit(GLuint textureUnit)
{
	assert(0 <= textureUnit && textureUnit <= (GL_TEXTURE31 - GL_TEXTURE0));

	glActiveTexture(textureUnit + GL_TEXTURE0);
	ASSERT_GL_ERROR_MACRO();
	glBindTexture(GL_TEXTURE_3D, m_handle);
	ASSERT_GL_ERROR_MACRO();
}

inline void Texture3d::Unbind()
{
	glBindTexture(GL_TEXTURE_3D, 0);
	ASSERT_GL_ERROR_MACRO();
}



