#include "stdafx.h"
#include "Shader.h"
#include <fstream>
#include <sstream>
#include <memory>

namespace
{
	std::string ReadFile(const char* file)
	{
		std::ifstream t(file);

		std::stringstream buffer;
		buffer << t.rdbuf();

		std::string fileContent = buffer.str();

		return fileContent;
	}
}

Shader::Shader(ShaderType type)
	: m_shaderType(GetShaderType(type))
{
}

void Shader::FreeResource()
{
	glDeleteShader(m_handle);
}

bool Shader::Create(const char* filename)
{
	std::string fileContent = ReadFile(filename);
	GLint length = fileContent.length();
	const char* data = fileContent.data();

	m_handle = glCreateShader(m_shaderType);
	glShaderSource(m_handle, 1, &data, &length);
	glCompileShader(m_handle);
	
	int wasCompiled = 0;
	glGetShaderiv(m_handle, GL_COMPILE_STATUS, &wasCompiled);
	if (wasCompiled == 0)
	{
		PrintCompileError();
		return false;
	}
	return true;
}

void Shader::PrintCompileError() const
{
	GLsizei logLength;
	glGetShaderiv(m_handle, GL_INFO_LOG_LENGTH, &logLength);
	// reserve one space for null terminator
	++logLength;

	std::unique_ptr<GLchar[]> log = std::make_unique<GLchar[]>(logLength);

	GLsizei writtenElements;
	glGetShaderInfoLog(m_handle, logLength, &writtenElements, log.get());

	assert((logLength) > writtenElements);
	log[logLength-1] = '\0';

	std::printf("Shader Error: %s", log.get());	
}
