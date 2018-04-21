#pragma once

#include "Shader.h"
#include "Resource.h"
#include "glew.h"
#include <unordered_map>
#include <string.h>
#include <vector>

class ShaderProgram : public Resource<ShaderProgram>
{
public:
	ShaderProgram();
	~ShaderProgram();
	void FreeResource();

	static GLuint GetCurrentProgramm();
	bool IsInUse();
	bool IsValid() const;

	void BindAttributeLocation(GLuint index, const char* name);
	void UseProgram();

	void UnuseProgram();

	bool CreateShaders(const char* vertexShaderFileName, const char* fragmentShaderFilename);
	bool CreateProgram();
	bool CreateAndAttachShader(const char* shaderFileName, ShaderType type);
	bool LinkShaders();
	void PrintProgramInfoLog() const;

	void FindUniforms(const std::vector<std::string>& UniformNames);
	void FindUniform(const std::string& UniformName);

	void SetMatrixUniform(const glm::mat4& matrix, const std::string& UniformName);
	void SetVec4Uniform(const glm::vec4 vector, const std::string& UniformName);
	void SetVec3Uniform(const glm::vec3 vector, const std::string& UniformName);
	void SetBoolUniform(bool value, const std::string& UniformName);
	void SetIntUniform(int value, const std::string & UniformName);
	void SetFloatUniform(float value, const std::string& UniformName);
	void SetSamplerTextureUnit(GLint textureUnit, const std::string UniformName);
	void SetTranformFeedback();

private:
	std::vector<Shader> m_shaders;

	std::unordered_map<std::string, GLuint> m_uniformLocations;
};