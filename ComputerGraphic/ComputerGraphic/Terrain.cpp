#include "stdafx.h"
#include "Terrain.h"

#include <string>
#include "Image.h"
#include "Camera.h"
#include "SpotLight.h"


namespace
{
	constexpr int quadVertices = 4;
	const std::string HalfExtentUniformName = "halfExtent";
	const std::string CenterUniformName = "center";
	const std::string TerrainNormalUniformName = "terrainNormal";
	const std::string TerrainTangentUniformName = "terrainTangent";
	const std::string TerrainBitangentUniformName = "terrainBitangent";

	const std::string HeightmapUniformName = "heightMap";
	const std::string MaxHeightUniformName = "maxHeight";

	const std::string InnerTessLevelUniformName = "innerTessLevel";
	const std::string OuterTessLevelUniformName = "outerTessLevel";

	const std::string ViewProjectionUniformName = "viewProjection";

	const std::array<std::string, 2> LightPosUniformNameArray = []()
	{
		std::array<std::string, 2> result;
		for (size_t i = 0; i < result.size(); ++i)
		{
			result[i] = std::string("lightPos[") + std::to_string(i) + std::string("]");
		}
		return result;
	}();
}


void Terrain::Init(glm::vec2 halfExtent, glm::vec3 center, float maxHeight, const char* heightmapFilePath)
{
	m_halfExtent = halfExtent;
	m_center = center;
	m_terrainNormal = glm::vec3(0.f, 1.f, 0.f);
	m_terrainTangent = glm::vec3(1.f, 0.f, 0.f);
	m_terrainBitangent = glm::vec3(0.f, 0.f, 1.f);
	m_maxHeight = maxHeight;

	m_terrainShaderProgram.CreateProgram();
	m_terrainShaderProgram.CreateAndAttachShader("../Shader/terrain.vert", ShaderType::Vertex);
	m_terrainShaderProgram.CreateAndAttachShader("../Shader/terrain.tesc", ShaderType::TesselationControll);
	m_terrainShaderProgram.CreateAndAttachShader("../Shader/terrain.tese", ShaderType::TesselationEvaluation);
	m_terrainShaderProgram.CreateAndAttachShader("../Shader/terrain.geo", ShaderType::Geometry);
	m_terrainShaderProgram.CreateAndAttachShader("../Shader/terrain.frag", ShaderType::Fragment);

	m_terrainShaderProgram.LinkShaders();
	m_terrainShaderProgram.FindUniforms({
		HalfExtentUniformName,
		CenterUniformName,
		TerrainNormalUniformName,
		TerrainTangentUniformName,
		TerrainBitangentUniformName,
		HeightmapUniformName,
		MaxHeightUniformName,
		InnerTessLevelUniformName,
		OuterTessLevelUniformName,
		ViewProjectionUniformName
		});

	for (size_t i = 0; i < LightPosUniformNameArray.size(); ++i)
	{
		m_terrainShaderProgram.FindUniform(LightPosUniformNameArray[i]);
	}

	Image image;
	image.LoadImage(heightmapFilePath);
	assert(image.GetNumberChannels() == 3);

	m_heightmap.Create();
	m_heightmap.Bind();
	m_heightmap.TextureImage(0, GL_RGB, image.GetWidth(), image.GetHeight(), GL_RGB, GL_UNSIGNED_BYTE, image.GetData());
	m_heightmap.SetLinearFiltering();
	m_heightmap.SetRepeating();
	m_heightmap.Unbind();

	m_emptyVao.Create();
}

void Terrain::Draw(const glm::mat4& viewProjectionMatrix, const SpotLight* lightPos, int lightCount)
{
	m_terrainShaderProgram.UseProgram();
	m_terrainShaderProgram.SetFloatUniform(m_maxHeight, MaxHeightUniformName);
	m_terrainShaderProgram.SetFloatUniform(1.f, InnerTessLevelUniformName);
	m_terrainShaderProgram.SetFloatUniform(1.f, OuterTessLevelUniformName);

	m_terrainShaderProgram.SetVec3Uniform(m_terrainNormal, TerrainNormalUniformName);
	m_terrainShaderProgram.SetVec3Uniform(m_terrainBitangent, TerrainBitangentUniformName);
	m_terrainShaderProgram.SetVec3Uniform(m_terrainTangent, TerrainTangentUniformName);
	m_terrainShaderProgram.SetVec3Uniform(m_center, CenterUniformName);

	m_terrainShaderProgram.SetVec2Uniform(m_halfExtent, HalfExtentUniformName);
	m_terrainShaderProgram.SetMatrixUniform(viewProjectionMatrix, ViewProjectionUniformName);

	for (int i = 0; i < lightCount; ++i)
	{
		m_terrainShaderProgram.SetVec3Uniform(lightPos[i].m_position, LightPosUniformNameArray[i]);
	}

	m_heightmap.Bind();
	constexpr int heightMapTextureUnit = 0;
	m_heightmap.BindToTextureUnit(heightMapTextureUnit);
	m_terrainShaderProgram.SetSamplerTextureUnit(heightMapTextureUnit, HeightmapUniformName);

	glPatchParameteri(GL_PATCH_VERTICES, quadVertices);
	m_emptyVao.Bind();

	glDisable(GL_CULL_FACE);
	glDrawArrays(GL_PATCHES, 0, quadVertices);
	glEnable(GL_CULL_FACE);

	m_emptyVao.Unbind();
	m_terrainShaderProgram.UnuseProgram();	
}
