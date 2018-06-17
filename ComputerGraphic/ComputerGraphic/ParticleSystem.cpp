#include "stdafx.h"
#include "ParticleSystem.h"
#include "TypeToGLEnum.h"
#include "Query.h"
#include "MeshData.h"
#include <random>

namespace
{
	const std::string DELTA_SECONDS_UNIFORM_NAME("deltaSeconds");

	const std::string ViewProjectionUniformName("viewProjection");
	const std::string MODEL_UNIFORM_NAME("model");
	const std::string INVERSE_TRANSPOSED_MODEL_MATRIX_UNIFORM_NAME("inverseTransposedModelMat");


	const int defaultDevisor = 0;
	const int advanceOncePerInstance = 1;
}


ParticleSystem::ParticleSystem()
	: m_maxParticles(0)
	, m_numCurrentParticles(0)
{

}

void ParticleSystem::Init(int maxParticles,const char* updateVert, const char* updateGeo, const char* drawVert, const char* drawFrag)
{
	m_maxParticles = maxParticles;
	const GLsizeiptr bufferSize = maxParticles * sizeof(ParticleData);
	for (AttributeBuffer& particleBuffer : m_particleBuffer)
	{
		AttributeBuffer newBuffer;
		newBuffer.Create();
		newBuffer.Bind();
		newBuffer.AllocateBufferData(bufferSize, GL_DYNAMIC_READ);

		newBuffer.Unbind();

		particleBuffer = std::move(newBuffer);
	}

	m_vao.Create();
	m_vao.Bind();
	m_vao.EnableAttribute(ParticleData::PositionLocation);
	m_vao.EnableAttribute(ParticleData::VelocityLocation);
	m_vao.EnableAttribute(ParticleData::SecondsToLiveLocation);
	m_vao.Unbind();

	// setup update shader
	{
		m_updateShader.CreateProgram();
		
		m_updateShader.CreateAndAttachShader(updateVert, ShaderType::Vertex);
		m_updateShader.CreateAndAttachShader(updateGeo, ShaderType::Geometry);

		m_updateShader.BindAttributeLocation(ParticleData::PositionLocation, ParticleData::POSITION_ATTRIBUTE_NAME);
		m_updateShader.BindAttributeLocation(ParticleData::VelocityLocation, ParticleData::VELOCITY_ATTRIBUTE_NAME);
		m_updateShader.BindAttributeLocation(ParticleData::SecondsToLiveLocation, ParticleData::SECONDS_TO_LIVE_ATTRIBUTE_NAME);

		const char* tranformFeedbackOutput[] = {
			"geo_out.Position",
			"geo_out.Velocity",
			"geo_out.SecondsToLive",
		};

		m_updateShader.SetTranformFeedback(tranformFeedbackOutput);

		m_updateShader.LinkShaders();
		m_updateShader.FindUniform(DELTA_SECONDS_UNIFORM_NAME);
	}

	// setup draw shader
	{
		m_drawShader.CreateProgram();
		m_drawShader.CreateAndAttachShader(drawVert, ShaderType::Vertex);
		m_drawShader.CreateAndAttachShader(drawFrag, ShaderType::Fragment);

		// particle data
		m_drawShader.BindAttributeLocation(ParticleData::PositionLocation, ParticleData::POSITION_ATTRIBUTE_NAME);
		m_drawShader.BindAttributeLocation(ParticleData::VelocityLocation, ParticleData::VELOCITY_ATTRIBUTE_NAME);
		m_drawShader.BindAttributeLocation(ParticleData::SecondsToLiveLocation, ParticleData::SECONDS_TO_LIVE_ATTRIBUTE_NAME);

		int particleLocations = ParticleData::Location::enum_Size;

		// mesh data
		m_drawShader.BindAttributeLocation(VertextAttribute::POSITION_ATTRIBUTE_LOCATION + particleLocations, VertextAttribute::POSITION_ATTRIBUTE_NAME);
		m_drawShader.BindAttributeLocation(VertextAttribute::NORMAL_ATTRIBUTE_LOCATION + particleLocations, VertextAttribute::NORMAL_ATTRIBUTE_NAME);
		m_drawShader.BindAttributeLocation(VertextAttribute::TANGENT_ATTRIBUTE_LOCATION + particleLocations, VertextAttribute::TANGENT_ATTRIBUTE_NAME);
		m_drawShader.BindAttributeLocation(VertextAttribute::BITANGENT_ATTRIBUTE_LOCATION + particleLocations, VertextAttribute::BITANGENT_ATTRIBUTE_NAME);
		m_drawShader.BindAttributeLocation(VertextAttribute::TEXTCOORD_ATTRIBUTE_LOCATION + particleLocations, VertextAttribute::TEXCOORD_ATTRIBUTE_NAME);

		m_drawShader.LinkShaders();
		m_drawShader.FindUniforms({
			ViewProjectionUniformName,
			MODEL_UNIFORM_NAME,
			INVERSE_TRANSPOSED_MODEL_MATRIX_UNIFORM_NAME,
		});
	}

	m_meshAttribsBuffer.Create();
}

void ParticleSystem::SetParticelMesh(const VertextAttribute* data, int numVertices)
{
	m_meshAttribsBuffer.Bind();
	m_meshAttribsBuffer.AllocateAndSetBufferData(data, numVertices);
	m_meshAttribsBuffer.Unbind();
	m_numMeshVertices = numVertices;
}

void ParticleSystem::GenerateRandomParticels(glm::vec3 location, int num)
{
	std::minstd_rand rand;
	std::uniform_real_distribution<float> positionDist(-1.f, 1.f);
	std::uniform_real_distribution<float> velocityDist(-50.f, 50.f);
	std::uniform_real_distribution<float> secondsToLiveDist(1.f, 10.f);

	std::vector<ParticleData> data;
	data.resize(num);
	for (int i = 0; i < num; ++i)
	{
		data[i].position = location + glm::vec3{ positionDist(rand), positionDist(rand), positionDist(rand) };
		data[i].velocity = glm::vec3{ positionDist(rand), 3.f , positionDist(rand)};
		data[i].secondsToLive = secondsToLiveDist(rand);
	}

	AddParticles(data.data(), data.size());
}

void ParticleSystem::AddParticles(const ParticleData* particles, int numberOfParticles)
{
	int realParticlesToWrite = std::min(numberOfParticles, m_maxParticles - m_numCurrentParticles);
	if (realParticlesToWrite < numberOfParticles)
	{
		std::printf("Not enough Space in Particle System. Only adding first %d of %d given particles", realParticlesToWrite, numberOfParticles);
	}
	
	assert(realParticlesToWrite <= numberOfParticles);

	GetReadBuffer().Bind();
	GetReadBuffer().SetBufferData(particles, realParticlesToWrite, m_numCurrentParticles);
	m_numCurrentParticles += realParticlesToWrite;
	glFlush();


	std::array<ParticleData, 200> outData;
	int dataToRead = std::min<int>(outData.size(), m_numCurrentParticles);
	GetReadBuffer().GetBufferData(outData.data(), dataToRead, 0);

	GetReadBuffer().Unbind();
}

void ParticleSystem::Update(float DeltaSeconds)
{
	if (m_numMeshVertices == 0)
	{
		return;
	}

#if 1
	m_vao.Bind();
	GetReadBuffer().Bind();
	BindReadBufferAttribsToVao();
	m_updateShader.UseProgram();
	m_updateShader.SetFloatUniform(DeltaSeconds, DELTA_SECONDS_UNIFORM_NAME);

	// Bind rockVertexBuffer to transform feedback, output from geometry shader will be saved in there
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, GetWriteBuffer().GetHandle());

	Query query;
	query.Create();
	query.Begin(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);

	glBeginTransformFeedback(GL_POINTS);
	ASSERT_GL_ERROR_MACRO();

	glEnable(GL_RASTERIZER_DISCARD);

	//glDrawArraysInstanced(GL_POINTS, 0, dummyPoints.size(), 1);
	glDrawArrays(GL_POINTS, 0, m_numCurrentParticles);

	// Reenable Rasterizer / fragment shader
	glDisable(GL_RASTERIZER_DISCARD);

	glEndTransformFeedback();
	ASSERT_GL_ERROR_MACRO();

	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, 0);

	query.End();
	m_numCurrentParticles = query.GetResult();	

	m_updateShader.UnuseProgram();
	GetReadBuffer().Unbind();
	m_vao.Unbind();
	SwitchParticleBuffers();
#endif
#if 0
	std::puts("Update:");
	GetReadBuffer().Bind();
	std::array<ParticleData, 200> outData;
	int dataToRead = std::min<int>(outData.size(), m_numCurrentParticles);
	GetReadBuffer().GetBufferData(outData.data(), dataToRead, 0);
	for (int i = 0; i < dataToRead; ++i)
	{
		std::printf("%d: pos(%f,%f,%f), vel(%f,%f,%f), secondsToLive(%f) \n", i,
			outData[i].position.x, outData[i].position.x, outData[i].position.z,
			outData[i].velocity.x, outData[i].velocity.x, outData[i].velocity.z,
			outData[i].secondsToLive);
	}
	GetReadBuffer().Unbind();
#endif
}

void ParticleSystem::Draw(const glm::mat4& ViewProjectionMatrix)
{
	if (m_numMeshVertices == 0)
	{
		return;
	}

	const glm::mat4 identityMat = glm::mat4();
	m_vao.Bind();

	GetReadBuffer().Bind();
	BindReadBufferAttribsToVao();
	GetReadBuffer().Unbind();

	m_meshAttribsBuffer.Bind();
	BindMeshAttribsToVao();
	m_meshAttribsBuffer.Unbind();

	m_drawShader.UseProgram();
	m_drawShader.SetMatrixUniform(identityMat, MODEL_UNIFORM_NAME);
	m_drawShader.SetMatrixUniform(identityMat, INVERSE_TRANSPOSED_MODEL_MATRIX_UNIFORM_NAME);

	m_drawShader.SetMatrixUniform(ViewProjectionMatrix, ViewProjectionUniformName);

	// Set it so that the gpu always uses the same particle Location for the whole instance
	glVertexAttribDivisor(ParticleData::PositionLocation, advanceOncePerInstance);
	glVertexAttribDivisor(ParticleData::VelocityLocation, advanceOncePerInstance);
	glVertexAttribDivisor(ParticleData::SecondsToLiveLocation, advanceOncePerInstance);

	//std::printf("rendering %i particles\n", m_numCurrentParticles);
	glDrawArraysInstanced(GL_TRIANGLES, 0, m_numMeshVertices, m_numCurrentParticles);

	// Restore behavior
	glVertexAttribDivisor(ParticleData::PositionLocation, defaultDevisor);
	glVertexAttribDivisor(ParticleData::VelocityLocation, defaultDevisor);
	glVertexAttribDivisor(ParticleData::SecondsToLiveLocation, defaultDevisor);

	m_drawShader.UnuseProgram();
	m_vao.Unbind();
}

void ParticleSystem::BindReadBufferAttribsToVao()
{
	GetReadBuffer().SetVertexAttributePtr(
		ParticleData::PositionLocation,
		decltype(ParticleData::position)::length(),
		TypeToGLEnum<decltype(ParticleData::position)::value_type>::value,
		sizeof(ParticleData),
		offsetof(ParticleData, position)
	);

	GetReadBuffer().SetVertexAttributePtr(
		ParticleData::VelocityLocation,
		decltype(ParticleData::velocity)::length(),
		TypeToGLEnum<decltype(ParticleData::velocity)::value_type>::value,
		sizeof(ParticleData),
		offsetof(ParticleData, velocity)
	);

	GetReadBuffer().SetVertexAttributePtr(
		ParticleData::SecondsToLiveLocation,
		1,
		GL_FLOAT,
		sizeof(ParticleData),
		offsetof(ParticleData, secondsToLive)
	);
}

void ParticleSystem::BindMeshAttribsToVao()
{
	int numPreviousLocations = ParticleData::Location::enum_Size;

	m_vao.EnableAttribute(VertextAttribute::POSITION_ATTRIBUTE_LOCATION + numPreviousLocations);
	m_meshAttribsBuffer.SetVertexAttributePtr(
		VertextAttribute::POSITION_ATTRIBUTE_LOCATION + numPreviousLocations,
		decltype(VertextAttribute::position)::length(),
		TypeToGLEnum<decltype(VertextAttribute::position)::value_type>::value,
		sizeof(VertextAttribute),
		offsetof(VertextAttribute, position)
	);

	m_vao.EnableAttribute(VertextAttribute::NORMAL_ATTRIBUTE_LOCATION + numPreviousLocations);
	m_meshAttribsBuffer.SetVertexAttributePtr(
		VertextAttribute::NORMAL_ATTRIBUTE_LOCATION + numPreviousLocations,
		decltype(VertextAttribute::normal)::length(),
		TypeToGLEnum<decltype(VertextAttribute::normal)::value_type>::value,
		sizeof(VertextAttribute),
		offsetof(VertextAttribute, normal)
	);

	m_vao.EnableAttribute(VertextAttribute::TANGENT_ATTRIBUTE_LOCATION + numPreviousLocations);
	m_meshAttribsBuffer.SetVertexAttributePtr(
		VertextAttribute::TANGENT_ATTRIBUTE_LOCATION + numPreviousLocations,
		decltype(VertextAttribute::tangent)::length(),
		TypeToGLEnum<decltype(VertextAttribute::tangent)::value_type>::value,
		sizeof(VertextAttribute),
		offsetof(VertextAttribute, tangent)
	);

	m_vao.EnableAttribute(VertextAttribute::BITANGENT_ATTRIBUTE_LOCATION + numPreviousLocations);
	m_meshAttribsBuffer.SetVertexAttributePtr(
		VertextAttribute::BITANGENT_ATTRIBUTE_LOCATION + numPreviousLocations,
		decltype(VertextAttribute::bitangent)::length(),
		TypeToGLEnum<decltype(VertextAttribute::bitangent)::value_type>::value,
		sizeof(VertextAttribute),
		offsetof(VertextAttribute, bitangent)
	);

	m_vao.EnableAttribute(VertextAttribute::TEXTCOORD_ATTRIBUTE_LOCATION + numPreviousLocations);
	m_meshAttribsBuffer.SetVertexAttributePtr(
		VertextAttribute::TEXTCOORD_ATTRIBUTE_LOCATION + numPreviousLocations,
		decltype(VertextAttribute::texcoord)::length(),
		TypeToGLEnum<decltype(VertextAttribute::texcoord)::value_type>::value,
		sizeof(VertextAttribute),
		offsetof(VertextAttribute, texcoord)
	);
}
