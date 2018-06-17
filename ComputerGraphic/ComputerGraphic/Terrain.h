#pragma once

#include "ShaderProgram.h"
#include "Texture.h"
#include "VertexArray.h"

class SpotLight;

class Terrain
{
public:
	void Init(glm::vec2 dimension, glm::vec3 center, float maxHeight, const char* heightmapFilePath, const char* texturePath);

	void Draw(const glm::mat4& viewProjectionMatrix, const SpotLight* lightPos, int lightCount);

	ShaderProgram m_terrainShaderProgram;

	glm::vec2 m_halfExtent;
	glm::vec3 m_center;

	glm::vec3 m_terrainNormal;
	glm::vec3 m_terrainTangent;
	glm::vec3 m_terrainBitangent;

	float m_maxHeight;
	float m_quality;

	Texture m_heightmap;
	Texture m_texture;


	VertexArray m_emptyVao;
};

