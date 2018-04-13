#pragma once

#include <type_traits>
#include "glew.h"


enum class BufferType
{
	Index,
	Attribute,
};

constexpr int GetBindBufferTarget(BufferType bufferType)
{
	switch (bufferType)
	{
	case BufferType::Index:
		return GL_ELEMENT_ARRAY_BUFFER;
	case BufferType::Attribute:
		return GL_ARRAY_BUFFER;
	default:
		assert(false);
		return -1;
	}
}

constexpr int GetBufferBinding(BufferType bufferType)
{
	switch (bufferType)
	{
	case BufferType::Index:
		return GL_ELEMENT_ARRAY_BUFFER_BINDING;
	case BufferType::Attribute:
		return GL_ARRAY_BUFFER_BINDING;
	default:
		assert(false);
		return -1;
	}
}
