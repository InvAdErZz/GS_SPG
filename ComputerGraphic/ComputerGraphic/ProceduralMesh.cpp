#include "stdafx.h"
#include "ProceduralMesh.h"
#include "glm.hpp"
#include "Query.h"

namespace
{
	
	constexpr glm::ivec3 Texture3dDimensions(96 , 96 , 256);
	constexpr glm::ivec3 Texture3dDimensionsMinOne(Texture3dDimensions.x - 1, Texture3dDimensions.y - 1, Texture3dDimensions.z - 1);
	constexpr glm::vec3 Inversed3dDimensions(1.f / Texture3dDimensions.x, 1.f / Texture3dDimensions.y, 1.f / Texture3dDimensions.z);
	constexpr int Texture3dNumSamples = Texture3dDimensions.x * Texture3dDimensions.y * Texture3dDimensions.z;
	constexpr float Texture3dSliceHeight = 0.5f;
	constexpr float Texture3dDistanceBetweenPoints = 0.5f;


	//constexpr glm::vec3 Inversed3dDimensionsMinusOne(1.f / Texture3dDimensionsMinOne.x, 1.f / Texture3dDimensionsMinOne.y, 1.f / Texture3dDimensionsMinOne.z);
	const std::string WS_VOXEL_SIZE_UNIFORM_NAME("worldSpaceVoxelSize");
	const std::string INVERSED_NUM_LAYERS_UNIFORM_NAME("inversedNumLayers");

	const std::string HIGHT_UNIFORM_NAME("height");
	const std::string DENSITY_TEXTURE_UNIFORM_NAME("densityTex");
	const std::string NORMAL_AMBIENT_TEXTURE_UNIFORM_NAME("normalAmbientTex");

	const std::string VIEW_PROJECTION_UNIFORM_NAME("viewProjection");
	const std::string COLOR_UNIFORM_NAME("color");
	const std::string MODEL_MATRIX_UNIFORM_NAME("model");

	const std::string INVERSED_3D_DIM_UNIFORM_NAME("inversedTexture3dDimensions");

	constexpr int DummyPointsNum = Texture3dDimensionsMinOne.x * Texture3dDimensionsMinOne.y;
	const std::vector<glm::vec2> dummyPoints = []()
	{
		std::vector<glm::vec2> result;
		result.reserve(DummyPointsNum);
		for (int x = 0; x < Texture3dDimensionsMinOne.x; ++x)
		{
			for (int y = 0; y < Texture3dDimensionsMinOne.y; ++y)
			{
				result.push_back(glm::vec2(x, y));
			}
		}
		assert(result.size() == DummyPointsNum);
		return result;
	}();
}


void ProceduralMesh::Init()
{
}

void ProceduralMesh::GenerateMesh(const LookupBuffer& lookupBuffer)
{
	// Create Density Texture
	Texture3d desityTexture;
	desityTexture.Create();
	desityTexture.Bind();
	desityTexture.TextureImage(0, GL_R32F, Texture3dDimensions.x, Texture3dDimensions.y, Texture3dDimensions.z, GL_RED, GL_FLOAT, nullptr);
	desityTexture.SetLinearFiltering();
	
	// Fill Density Texture
	{
		ShaderProgram densityShader;
		if (!densityShader.CreateShaders("../Shader/texture3d.vert", "../Shader/texture3d.frag"))
		{
			assert(false);
		}

		densityShader.LinkShaders();
		densityShader.FindUniforms({
			HIGHT_UNIFORM_NAME
			});

		densityShader.UseProgram();
		FrameBuffer densityFrameBuffer;
		densityFrameBuffer.Create();
		densityFrameBuffer.Bind();

		glViewport(0, 0, Texture3dDimensions.x, Texture3dDimensions.y);
	
		for (size_t i = 0; i < Texture3dDimensions.z; i++)
		{

			densityFrameBuffer.BindTexture3D(GL_COLOR_ATTACHMENT0, desityTexture.GetHandle(), 0, i);
			
			densityShader.SetFloatUniform(i * Inversed3dDimensions.z, HIGHT_UNIFORM_NAME);
			
			// we don't need a vbo or vao as we don't need any inputdata. We are just drawing a screen triangle. Look inside shader for more.
			glDrawArrays(GL_TRIANGLES, 0, 3);
			ASSERT_GL_ERROR_MACRO();
		}
		
		densityFrameBuffer.Unbind();
		densityShader.UnuseProgram();
	}

	// Create NormalAmbient Texture
	Texture3d normalAmbientTexture;
	normalAmbientTexture.Create();
	normalAmbientTexture.Bind();
	normalAmbientTexture.TextureImage(0, GL_RGBA, Texture3dDimensions.x, Texture3dDimensions.y, Texture3dDimensions.z, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	normalAmbientTexture.SetNearestNeighbourFiltering();

	 // Fill  NormalAmbient Texture
	 {
		ShaderProgram normalAmbientShader;
		if (!normalAmbientShader.CreateShaders("../Shader/normalAmbient.vert", "../Shader/normalAmbient.frag"))
		{
			assert(false);
		}

		normalAmbientShader.LinkShaders();
		normalAmbientShader.FindUniforms({
			HIGHT_UNIFORM_NAME,
			DENSITY_TEXTURE_UNIFORM_NAME,
			INVERSED_3D_DIM_UNIFORM_NAME
			});

		normalAmbientShader.UseProgram();

		FrameBuffer normalAmbientFramebuffer;
		normalAmbientFramebuffer.Create();
		normalAmbientFramebuffer.Bind();

		normalAmbientShader.SetSamplerTextureUnit(0, DENSITY_TEXTURE_UNIFORM_NAME);
		normalAmbientShader.SetVec3Uniform(Inversed3dDimensions, INVERSED_3D_DIM_UNIFORM_NAME);
		desityTexture.BindToTextureUnit(0);

		glViewport(0, 0, Texture3dDimensions.x, Texture3dDimensions.y);

		for (size_t i = 0; i < Texture3dDimensions.z; i++)
		{
			normalAmbientFramebuffer.BindTexture3D(GL_COLOR_ATTACHMENT0, normalAmbientTexture.GetHandle(), 0, i);
			normalAmbientShader.SetFloatUniform(i * Inversed3dDimensions.z, HIGHT_UNIFORM_NAME);

			glClear(GL_COLOR_BUFFER_BIT);
			ASSERT_GL_ERROR_MACRO();
			// we don't need a vbo or vao as we don't need any inputdata. We are just drawing a screen triangle. Look inside shader for more.
			glDrawArrays(GL_TRIANGLES, 0, 3);
			ASSERT_GL_ERROR_MACRO();
		}

		normalAmbientFramebuffer.Unbind();
		normalAmbientShader.UnuseProgram();
	}



	// create buffer for rock vertices
	AttributeBuffer rockVertexBuffer;
	rockVertexBuffer.Create();
	rockVertexBuffer.Bind();
	// reserve enough space
	// 5 triangles 
	// vertex
	// finaly make it a bit smaller
	constexpr int rockBufferbytes = Texture3dNumSamples * 5 * 3 * sizeof(ProceduralMeshVertex) / 50;

	rockVertexBuffer.AllocateBufferData(rockBufferbytes, GL_STATIC_READ);

	GLuint numRockPrimitives = 0;

	{
		ShaderProgram marchingCubesShader;
		marchingCubesShader.CreateProgram();
		marchingCubesShader.CreateAndAttachShader("../Shader/mc.vert", ShaderType::Vertex);
		marchingCubesShader.CreateAndAttachShader("../Shader/mc.geo", ShaderType::Geometry);
		marchingCubesShader.BindAttributeLocation(0, "in_Position");
		const char* tranformFeedbackOutput[] = { 
			"geo_out.position"
			, "geo_out.normal" 
			, "geo_out.texcoord"
		};
		marchingCubesShader.SetTranformFeedback(tranformFeedbackOutput);

		marchingCubesShader.LinkShaders();
		marchingCubesShader.FindUniforms({ 
			WS_VOXEL_SIZE_UNIFORM_NAME,
			DENSITY_TEXTURE_UNIFORM_NAME,
			INVERSED_3D_DIM_UNIFORM_NAME,
			NORMAL_AMBIENT_TEXTURE_UNIFORM_NAME });

		// Setup shader Uniforms
		// Enable using the lookup buffer data to be used in the marching cubes shader
		marchingCubesShader.UseProgram();
		lookupBuffer.UpdateUniforms(marchingCubesShader);
		marchingCubesShader.SetSamplerTextureUnit(0, DENSITY_TEXTURE_UNIFORM_NAME);
		desityTexture.BindToTextureUnit(0);

		marchingCubesShader.SetSamplerTextureUnit(1, NORMAL_AMBIENT_TEXTURE_UNIFORM_NAME);
		normalAmbientTexture.BindToTextureUnit(1);

		marchingCubesShader.SetFloatUniform(1.f, WS_VOXEL_SIZE_UNIFORM_NAME);
		marchingCubesShader.SetVec3Uniform(Inversed3dDimensions, INVERSED_3D_DIM_UNIFORM_NAME);
		
		VertexArray mcVao;
		mcVao.Create();
		mcVao.Bind();
		mcVao.EnableAttribute(0);

		printf("MC VAO %d:\n ", mcVao.GetHandle());

		AttributeBuffer pointCloudBuffer;
		pointCloudBuffer.Create();
		pointCloudBuffer.Bind();
		pointCloudBuffer.UploadBufferData(dummyPoints);

		pointCloudBuffer.SetVertexAttributePtr(0, glm::vec2::length(), GL_FLOAT, sizeof(glm::vec2), 0);

		printf("pointCloudBuffer %d:\n ", pointCloudBuffer.GetHandle());

		GLsizei buffSize = dummyPoints.size() * glm::vec2::length();
		GLfloat*  buffData = new GLfloat[buffSize];
		glGetBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * buffSize, buffData);

		for (int i = 0; i < buffSize; i+=2)
		{
			if (i % 90 != 0)
			{
				continue;
			}
			//printf("point %d: %d,%d, \n ",i/2, buffData[i], buffData[i+1]);
		}
		delete[] buffData;

		glFlush();


		// Bind rockVertexBuffer to transform feedback, output from geometry shader will be saved in there
		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, rockVertexBuffer.GetHandle());

		Query query;
		query.Create();
		query.Begin(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);

		glBeginTransformFeedback(GL_TRIANGLES);
		ASSERT_GL_ERROR_MACRO();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Disable Rasterizer / fragment shader
		glEnable(GL_RASTERIZER_DISCARD);

		assert(dummyPoints.size() == Texture3dDimensionsMinOne.y * Texture3dDimensionsMinOne.x);

		//glDrawArraysInstanced(GL_POINTS, 0, dummyPoints.size(), 1);
		glDrawArraysInstanced(GL_POINTS, 0, dummyPoints.size(), Texture3dDimensionsMinOne.z);

		// Reenable Rasterizer / fragment shader
		glDisable(GL_RASTERIZER_DISCARD);

		glEndTransformFeedback();
		ASSERT_GL_ERROR_MACRO();

		query.End();
		numRockPrimitives = query.GetResult();
	
		printf("%u primitives written!\n\n", numRockPrimitives);
		GLuint numRockVertices = numRockPrimitives * 3;
		GLuint numRockFloats = numRockVertices * 6;
		pointCloudBuffer.Unbind();
		mcVao.Unbind();
		marchingCubesShader.UnuseProgram();

		GLfloat*  feedback=  new GLfloat[numRockFloats];
		glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, sizeof(GLfloat) * numRockFloats, feedback);
		for (int i = 0; i < numRockFloats; i+=18)
		{
			if (i % 9000 != 0)
			{
				continue;
			}
			printf("tri %d: v %f,%f,%f | %f,%f,%f | %f,%f,%f \n ",i/18, feedback[i], feedback[i+1], feedback[i+2], feedback[i+6], feedback[i+7], feedback[i+8], feedback[i+12], feedback[i+13], feedback[i+14]);
		}
		delete[] feedback;

	}

	m_rockVertices = std::move(rockVertexBuffer);
	m_numRockTriangles = numRockPrimitives;
}

void ProceduralMesh::Render()
{
	VertexArray renderVao;
	renderVao.Create();
	renderVao.Bind();
	renderVao.EnableAttribute(ProceduralMeshVertex::PositionLocation);
	renderVao.EnableAttribute(ProceduralMeshVertex::NormalLocation);
	renderVao.EnableAttribute(ProceduralMeshVertex::TexcoordLocation);

	m_rockVertices.Bind();
	m_rockVertices.SetVertexAttributePtr(ProceduralMeshVertex::PositionLocation, decltype(ProceduralMeshVertex::position)::length(),
		GL_FLOAT, sizeof(ProceduralMeshVertex), offsetof(ProceduralMeshVertex, position));

	m_rockVertices.SetVertexAttributePtr(ProceduralMeshVertex::NormalLocation, decltype(ProceduralMeshVertex::normal)::length(),
		GL_FLOAT, sizeof(ProceduralMeshVertex), offsetof(ProceduralMeshVertex, normal));

	m_rockVertices.SetVertexAttributePtr(ProceduralMeshVertex::TexcoordLocation, decltype(ProceduralMeshVertex::texcoord)::length(),
		GL_FLOAT, sizeof(ProceduralMeshVertex), offsetof(ProceduralMeshVertex, texcoord));

	glDrawArrays(GL_TRIANGLES, 0, m_numRockTriangles * 3);
	m_rockVertices.Unbind();
	renderVao.Unbind();
}
