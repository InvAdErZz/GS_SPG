#pragma once
#include "ShaderType.h"
#include "Resource.h"


class Shader : public Resource<Shader>
{
public:
	Shader(ShaderType type);

	bool Create(const char* filename);
	void PrintCompileError() const;
	void FreeResource();
private:
	GLenum m_shaderType;
};

