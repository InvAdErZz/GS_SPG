#pragma once
#include <vec3.hpp>
#include <glew.h>
#include <array>

#include "ShaderProgram.h"
#include "VertexArray.h"
#include "AttributeBuffer.h"

struct VertextAttribute;
struct ParticleData
{
	glm::vec3 position;
	glm::vec3 velocity;
	GLfloat secondsToLive;

	enum Location
	{
		PositionLocation = 0,
		VelocityLocation,
		SecondsToLiveLocation,
		enum_Size
	};

	static constexpr const char* POSITION_ATTRIBUTE_NAME = "in_ParticlePosition";
	static constexpr const char* VELOCITY_ATTRIBUTE_NAME = "in_ParticleVelocity";
	static constexpr const char* SECONDS_TO_LIVE_ATTRIBUTE_NAME = "in_ParticleSecondsToLive";
};

class ParticleSystem
{
public:
	ParticleSystem();
	void Init(int maxParticles, const char* updateVert, const char* updateGeo, const char* drawVert, const char* drawFrag);
	void SetParticelMesh(const VertextAttribute* data, int numVertices);

	void GenerateRandomParticels(glm::vec3 location, int num);
	void AddParticles(const ParticleData* particles, int numberOfParticles);
	void Update(float DeltaSeconds);
	void Draw(const glm::mat4& ViewProjectionMatrix);
	void UseDrawShader() { m_drawShader.UseProgram(); }
	ShaderProgram& GetShader() { return m_drawShader; }

private:
	void BindReadBufferAttribsToVao();
	void BindMeshAttribsToVao();
	AttributeBuffer & GetReadBuffer() { return m_particleBuffer[m_currentReadBufferIndex]; }
	AttributeBuffer & GetWriteBuffer() { return m_particleBuffer[m_currentReadBufferIndex ^ 1]; }
	void SwitchParticleBuffers() { m_currentReadBufferIndex ^= 1; }

	ShaderProgram m_updateShader;
	ShaderProgram m_drawShader;
	VertexArray m_vao;
	std::array<AttributeBuffer, 2> m_particleBuffer;
	AttributeBuffer m_meshAttribsBuffer;

	int m_numMeshVertices;
	int m_currentReadBufferIndex;
	int m_maxParticles;
	int m_numCurrentParticles;
};


