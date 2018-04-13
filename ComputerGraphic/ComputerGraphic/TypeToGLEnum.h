#pragma once
#include "glew.h"
#include <type_traits>

template<class TYPE>
struct TypeToGLEnum
{
	static_assert(sizeof(TYPE) > 0, "TYPE not supported");
};

template<>
struct TypeToGLEnum<float> : public std::integral_constant<GLenum, GL_FLOAT>
{};

template<>
struct TypeToGLEnum<int> : public std::integral_constant<GLenum, GL_INT>
{};

template<>
struct TypeToGLEnum<unsigned int> : public std::integral_constant<GLenum, GL_UNSIGNED_INT>
{};