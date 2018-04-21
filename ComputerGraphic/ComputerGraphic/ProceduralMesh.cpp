#include "stdafx.h"
#include "ProceduralMesh.h"
#include "glm.hpp"
#include "Query.h"

namespace
{
	
	constexpr glm::ivec3 Texture3dDimensions(96, 96, 256);
	constexpr glm::ivec3 Texture3dDimensionsMinOne(Texture3dDimensions.x - 1, Texture3dDimensions.y - 1, Texture3dDimensions.z - 1);
	constexpr glm::vec3 Inversed3dDimensions(1.f / Texture3dDimensions.x, 1.f / Texture3dDimensions.y, 1.f / Texture3dDimensions.z);
	constexpr int Texture3dNumSamples = Texture3dDimensions.x * Texture3dDimensions.y * Texture3dDimensions.z;
	constexpr float Texture3dSliceHeight = 0.5f;
	constexpr float Texture3dDistanceBetweenPoints = 0.5f;


	constexpr glm::vec3 Inversed3dDimensionsMinusOne(1.f / Texture3dDimensionsMinOne.x, 1.f / Texture3dDimensionsMinOne.y, 1.f / Texture3dDimensionsMinOne.z);
	const std::string WS_VOXEL_SIZE_UNIFORM_NAME("worldSpaceVoxelSize");

	const std::string HIGHT_UNIFORM_NAME("height");
	const std::string DENSITY_TEXTURE_UNIFORM_NAME("densityTex");

	const std::string VIEW_PROJECTION_UNIFORM_NAME("viewProjection");
	const std::string COLOR_UNIFORM_NAME("color");
	const std::string MODEL_MATRIX_UNIFORM_NAME("model");


	std::array<glm::vec3, 3> screenTriangle = { 
		glm::vec3{ -1.0f, -1.0f, 0.f},
		glm::vec3{ 3.f, -1.f, 0.f },
		glm::vec3{ -1.f, 3.f, 0.f }
	};



	constexpr int DummyPointsNum = Texture3dDimensionsMinOne.x * Texture3dDimensionsMinOne.y * Texture3dDimensionsMinOne.z;
	const std::vector<glm::vec3> dummyPoints = []()
	{
		std::vector<glm::vec3> result;
		result.reserve(DummyPointsNum);
		for (int x = 1; x <= Texture3dDimensionsMinOne.x; ++x)
		{
			for (int y = 1; y <= Texture3dDimensionsMinOne.y; ++y)
			{
				for (int z = 1; z <= Texture3dDimensionsMinOne.z; ++z)
				{
#if 1
					result.push_back(glm::vec3(
						x * Inversed3dDimensions.x,
						y * Inversed3dDimensions.y,
						z * Inversed3dDimensions.z)
					);
#else
					result.push_back(glm::vec3(x,y,	z));
#endif
				}
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
	desityTexture.SetNearestNeighbourFiltering();

	// Fill Density Texture
	{
		ShaderProgram densityShader;
		if (!densityShader.CreateShaders("texture3d.vert", "texture3d.frag"))
		{
			assert(false);
		}

		densityShader.BindAttributeLocation(0, "in_Position");
		densityShader.LinkShaders();
		densityShader.FindUniforms({
			HIGHT_UNIFORM_NAME
			});

		AttributeBuffer screenTriangleBuffer;
		screenTriangleBuffer.Create();
		screenTriangleBuffer.Bind();
		screenTriangleBuffer.UploadBufferData(screenTriangle);

		VertexArray densityVao;
		densityVao.Create();
		densityVao.Bind();

		// Set position vertices
		screenTriangleBuffer.SetVertexAttributePtr(0, glm::vec3::length(), GL_FLOAT, sizeof(glm::vec3), 0);
		densityVao.EnableAttribute(0);
		densityShader.UseProgram();
		FrameBuffer densityFrameBuffer;
		densityFrameBuffer.Create();
		densityFrameBuffer.Bind();
		//densityFrameBuffer.BindTexture(GL_COLOR_ATTACHMENT0, desityTexture.GetHandle(), 0);

		glViewport(0, 0, Texture3dDimensions.x, Texture3dDimensions.y);
	

		int test = 0;
		for (size_t i = 0; i < 256; i++)
		{

			densityFrameBuffer.BindTexture3D(GL_COLOR_ATTACHMENT0, desityTexture.GetHandle(), 0, i);
			
			densityShader.SetFloatUniform(++test, HIGHT_UNIFORM_NAME);
			
			glClear(GL_COLOR_BUFFER_BIT);
			ASSERT_GL_ERROR_MACRO();
			glDrawArrays(GL_TRIANGLES, 0, 3);
			ASSERT_GL_ERROR_MACRO();
		}

		//glDrawArraysInstanced(GL_TRIANGLES, 0, 6, Texture3dDimensions.z);
		
		densityFrameBuffer.Unbind();
		screenTriangleBuffer.Unbind();
		densityVao.Unbind();
		densityShader.UnuseProgram();
	}

	// create buffer for rock vertices
	AttributeBuffer rockVertexBuffer;
	rockVertexBuffer.Create();
	rockVertexBuffer.Bind();
	// reserve enough space
	rockVertexBuffer.AllocateBufferData(Texture3dNumSamples * sizeof(glm::vec3) * 3, GL_STATIC_READ);

	GLuint numRockVertices = 0;

	{
		AttributeBuffer pointCloudBuffer;
		pointCloudBuffer.Create();
		pointCloudBuffer.Bind();
		pointCloudBuffer.UploadBufferData(screenTriangle);

		VertexArray mcVao;
		mcVao.Create();
		mcVao.Bind();
		pointCloudBuffer.SetVertexAttributePtr(0, glm::vec3::length(), GL_FLOAT, sizeof(glm::vec3), 0);


		ShaderProgram marchingCubesShader;
		marchingCubesShader.CreateProgram();
		marchingCubesShader.CreateAndAttachShader("mc.vert", ShaderType::Vertex);
		marchingCubesShader.CreateAndAttachShader("mc.geo", ShaderType::Geometry);
		marchingCubesShader.BindAttributeLocation(0, "in_Position");
		marchingCubesShader.SetTranformFeedback();
		marchingCubesShader.LinkShaders();
		marchingCubesShader.FindUniforms({ WS_VOXEL_SIZE_UNIFORM_NAME,DENSITY_TEXTURE_UNIFORM_NAME });

		// Setup shader Uniforms
		// Enable using the lookup buffer data to be used in the marching cubes shader
		marchingCubesShader.UseProgram();
		lookupBuffer.UpdateUniforms(marchingCubesShader);
		marchingCubesShader.SetSamplerTextureUnit(0, DENSITY_TEXTURE_UNIFORM_NAME);
		desityTexture.BindToTextureUnit(0);

		marchingCubesShader.SetFloatUniform(Inversed3dDimensions.x, WS_VOXEL_SIZE_UNIFORM_NAME);


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

		glDrawArrays(GL_POINTS, 0, Texture3dNumSamples);

		// Reenable Rasterizer / fragment shader
		glDisable(GL_RASTERIZER_DISCARD);

		glEndTransformFeedback();
		ASSERT_GL_ERROR_MACRO();

		query.End();
		numRockVertices = query.GetResult();

		printf("%u primitives written!\n\n", numRockVertices);

		pointCloudBuffer.Unbind();
		mcVao.Unbind();
		marchingCubesShader.UnuseProgram();

		GLfloat*  feedback=  new GLfloat[numRockVertices];
		glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, sizeof(float) * numRockVertices, feedback);
		for (int i = 0; i < numRockVertices; ++i)
		{
			if (i % 20 != 0)
			{
				continue;
			}
			printf("%f ", feedback[i]);
		}
		delete[] feedback;

	}

	m_rockVertices = std::move(rockVertexBuffer);
	m_numRockTriangles = numRockVertices;
}

void ProceduralMesh::Render()
{

}
