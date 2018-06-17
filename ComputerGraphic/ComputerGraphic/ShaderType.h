#pragma once

#include <type_traits>
#include "glew.h"
#include <assert.h>

enum class ShaderType
{
	Vertex,
	Fragment,
	Geometry,
	TesselationControll,
	TesselationEvaluation
};

constexpr GLenum GetShaderType(ShaderType shaderType)
{
	switch (shaderType)
	{
	case ShaderType::Vertex:
		return GL_VERTEX_SHADER;
	case ShaderType::Fragment:
		return GL_FRAGMENT_SHADER;
	case ShaderType::Geometry:
		return GL_GEOMETRY_SHADER;
	case ShaderType::TesselationControll:
		return GL_TESS_CONTROL_SHADER;
	case ShaderType::TesselationEvaluation:
		return GL_TESS_EVALUATION_SHADER;
	default:
		assert(false);
		return std::numeric_limits<GLenum>::max();
	}
}