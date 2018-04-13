#pragma once

#include <type_traits>
#include "glew.h"
#include <assert.h>

enum class ShaderType
{
	Vertex,
	Fragment,
};

constexpr GLenum GetShaderType(ShaderType shaderType)
{
	switch (shaderType)
	{
	case ShaderType::Vertex:
		return GL_VERTEX_SHADER;
	case ShaderType::Fragment:
		return GL_FRAGMENT_SHADER;
	default:
		assert(false);
		return std::numeric_limits<GLenum>::max();
	}
}