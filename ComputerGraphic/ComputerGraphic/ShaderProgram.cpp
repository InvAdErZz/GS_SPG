#include "stdafx.h"
#include "ShaderProgram.h"
#include "CheckGl.h"

ShaderProgram::ShaderProgram()
{}

ShaderProgram::~ShaderProgram()
{
	//assert(!IsInUse());
	for (Shader& shader : m_shaders)
	{
		if (shader.GetHandle() != 0)
		{
			glDetachShader(m_handle, shader.GetHandle());
			ASSERT_GL_ERROR_MACRO();
		}
	}
}

void ShaderProgram::FreeResource()
{
	assert(Resource::IsValid());
	glDeleteProgram(m_handle);
	ASSERT_GL_ERROR_MACRO();
}

GLuint ShaderProgram::GetCurrentProgramm()
{
	GLint result;
	glGetIntegerv(GL_CURRENT_PROGRAM, &result);
	ASSERT_GL_ERROR_MACRO();
	return static_cast<GLuint>(result);	
}

bool ShaderProgram::IsInUse()
{
	return GetCurrentProgramm() == m_handle;
}

bool ShaderProgram::IsValid() const
{
	Resource::IsValid();
	glValidateProgram(m_handle);
	ASSERT_GL_ERROR_MACRO();
	GLint validateStatus;
	glGetProgramiv(m_handle, GL_VALIDATE_STATUS, &validateStatus);
	ASSERT_GL_ERROR_MACRO();

	bool success = validateStatus == GL_TRUE;
	if (!success)
	{
		PrintProgramInfoLog();
	}
	return success;
}

void ShaderProgram::BindAttributeLocation(GLuint index, const char * name)
{
	glBindAttribLocation(m_handle, index, name);
	ASSERT_GL_ERROR_MACRO();
}

void ShaderProgram::UseProgram()
{
	assert(m_handle != 0);
	glUseProgram(m_handle);
	ASSERT_GL_ERROR_MACRO();
}

void ShaderProgram::UnuseProgram()
{
	assert(IsInUse());
	glUseProgram(0);
	ASSERT_GL_ERROR_MACRO();
}

bool ShaderProgram::CreateShaders(const char * vertexShaderFileName, const char * fragmentShaderFilename)
{
	if (!CreateProgram())
	{
		return false;
	}
	if (!CreateAndAttachShader(vertexShaderFileName, ShaderType::Vertex))
	{
		return false;
	}
	if (!CreateAndAttachShader(fragmentShaderFilename, ShaderType::Fragment))
	{
		return false;
	}

	return true;
}

bool ShaderProgram::CreateProgram()
{
	m_handle = glCreateProgram();
	ASSERT_GL_ERROR_MACRO();
	return true;
}

bool ShaderProgram::CreateAndAttachShader(const char* shaderFileName, ShaderType type)
{
	assert(Resource::IsValid());
	m_shaders.push_back(Shader(type));
	Shader& currentShader = m_shaders.back();
	if (!currentShader.Create(shaderFileName))
	{
		printf("Shader Creation Failed!");
		m_shaders.pop_back();
		return false;
	}
	glAttachShader(m_handle, currentShader.GetHandle());
	ASSERT_GL_ERROR_MACRO();
	return true;
}

bool ShaderProgram::LinkShaders()
{
	glLinkProgram(m_handle);
	ASSERT_GL_ERROR_MACRO();

	GLint hasLinkSucceded;
	glGetProgramiv(m_handle, GL_LINK_STATUS, &hasLinkSucceded);
	ASSERT_GL_ERROR_MACRO();

	if (hasLinkSucceded == 0)
	{
		PrintProgramInfoLog();
		return false;
	}

	return true;
}



void ShaderProgram::PrintProgramInfoLog() const
{
	GLsizei logLength;
	glGetProgramiv(m_handle, GL_INFO_LOG_LENGTH, &logLength);
	// reserve one space for null terminator
	++logLength;

	std::unique_ptr<GLchar[]> log = std::make_unique<GLchar[]>(logLength);

	GLsizei writtenElements;
	glGetProgramInfoLog(m_handle, logLength, &writtenElements, log.get());
	ASSERT_GL_ERROR_MACRO();

	assert((logLength) > writtenElements);
	log[logLength - 1] = '\0';

	std::printf("Linking Error: %s", log.get());
}


void ShaderProgram::FindUniforms(const std::vector<std::string>& UniformNames)
{
	for (const auto& name : UniformNames)
	{
		FindUniform(name);
	}
}

void ShaderProgram::FindUniform(const std::string& UniformName)
{
	GLuint location = glGetUniformLocation(m_handle, UniformName.c_str());
	ASSERT_GL_ERROR_MACRO();
	m_uniformLocations[UniformName] = location;
}

void ShaderProgram::SetMatrixUniform(const glm::mat4& matrix, const std::string& UniformName)
{
	assert(IsInUse());
	auto foundItem = m_uniformLocations.find(UniformName);
	assert(foundItem != m_uniformLocations.end());

	GLuint uniformLocation = foundItem->second;
	glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(matrix));
	ASSERT_GL_ERROR_MACRO();

}

void ShaderProgram::SetVec4Uniform(const glm::vec4 vector, const std::string & UniformName)
{
	assert(IsInUse());
	auto foundItem = m_uniformLocations.find(UniformName);
	assert(foundItem != m_uniformLocations.end());

	GLuint uniformLocation = foundItem->second;
	glUniform4f(uniformLocation, vector.x, vector.y, vector.z, vector.w);
	ASSERT_GL_ERROR_MACRO();
}

void ShaderProgram::SetVec3Uniform(const glm::vec3 vector, const std::string & UniformName)
{
	assert(IsInUse());
	auto foundItem = m_uniformLocations.find(UniformName);
	assert(foundItem != m_uniformLocations.end());

	GLuint uniformLocation = foundItem->second;
	glUniform3f(uniformLocation, vector.x, vector.y, vector.z);
	ASSERT_GL_ERROR_MACRO();
}

void ShaderProgram::SetBoolUniform(bool value, const std::string & UniformName)
{
	assert(IsInUse());
	auto foundItem = m_uniformLocations.find(UniformName);
	assert(foundItem != m_uniformLocations.end());

	GLuint uniformLocation = foundItem->second;
	glUniform1i(uniformLocation, value);
	ASSERT_GL_ERROR_MACRO();
}

void ShaderProgram::SetIntUniform(int value, const std::string & UniformName)
{
	assert(IsInUse());
	auto foundItem = m_uniformLocations.find(UniformName);
	assert(foundItem != m_uniformLocations.end());

	GLuint uniformLocation = foundItem->second;
	glUniform1i(uniformLocation, value);
	ASSERT_GL_ERROR_MACRO();
}

void ShaderProgram::SetFloatUniform(float value, const std::string& UniformName)
{
	assert(IsInUse());
	auto foundItem = m_uniformLocations.find(UniformName);
	assert(foundItem != m_uniformLocations.end());

	GLuint uniformLocation = foundItem->second;

	glUniform1f(uniformLocation, value);
	ASSERT_GL_ERROR_MACRO();
}

void ShaderProgram::SetSamplerTextureUnit(GLint textureUnit, const std::string UniformName)
{
	assert(IsInUse());
	auto foundItem = m_uniformLocations.find(UniformName);
	assert(foundItem != m_uniformLocations.end());

	GLuint uniformLocation = foundItem->second;
	glUniform1i(uniformLocation, textureUnit);
	ASSERT_GL_ERROR_MACRO();
}


