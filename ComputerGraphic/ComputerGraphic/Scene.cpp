#include "stdafx.h"
#include "Scene.h"
#include "InputManager.h"
#include "gtx/spline.hpp"
#include "gtx/quaternion.hpp"
#include <string>
#include "Image.h"

namespace 
{
	const glm::mat4 biasMatrix(
		0.5, 0.0, 0.0, 0.0,
		0.0, 0.5, 0.0, 0.0,
		0.0, 0.0, 0.5, 0.0,
		0.5, 0.5, 0.5, 1.0
	);

	constexpr glm::ivec3 Texture3dDimensions(96,96,256);
	constexpr glm::ivec3 Texture3dDimensionsMinOne(Texture3dDimensions.x - 1, Texture3dDimensions.y - 1, Texture3dDimensions.z-1);
	constexpr glm::vec3 Inversed3dDimensions(1.f / Texture3dDimensions.x, 1.f / Texture3dDimensions.y, 1.f / Texture3dDimensions.z);
	constexpr glm::vec3 Inversed3dDimensionsMinusOne(1.f / Texture3dDimensionsMinOne.x, 1.f / Texture3dDimensionsMinOne.y, 1.f / Texture3dDimensionsMinOne.z);

	constexpr int Texture3dNumSamples = Texture3dDimensions.x * Texture3dDimensions.y * Texture3dDimensions.z;
	constexpr float Texture3dSliceHeight = 0.5f;
	constexpr float Texture3dDistanceBetweenPoints = 0.5f;

	const glm::ivec2 shadowDimensions(1024, 1024);

	const std::string VIEW_PROJECTION_UNIFORM_NAME("viewProjection");
	const std::string COLOR_UNIFORM_NAME("color");
	const std::string MODEL_MATRIX_UNIFORM_NAME("model");
	const std::string RENDERED_SCENE_TEXTURE_UNIFORM_NAME("renderedscene");
	const std::string INVERSE_TRANSPOSED_MODEL_MATRIX_UNIFORM_NAME("inverseTransposedModelMat");
	const std::string CAMERA_POS_UNIFORM_NAME("cameraPos");
	const std::string NORMAL_MAP_UNIFORM_NAME("normalMap");
	const std::string ROUGHNESS_UNIFORM_NAME("roughness");
	const std::string WS_VOXEL_SIZE_UNIFORM_NAME("worldSpaceVoxelSize");

	const std::string HIGHT_UNIFORM_NAME("height");
	const std::string DENSITY_TEXTURE_UNIFORM_NAME("densityTex");




	const std::array<std::string, Scene::LightCount> SHADOW_MAP_UNIFORM_NAME_ARRAY = []()
	{
		std::array<std::string, Scene::LightCount> result;
		for (size_t i = 0; i < result.size(); ++i)
		{		
			result[i] = std::string("shadowmap[") + std::to_string(i) + std::string("]");
		}
		return result;

	}();

	const std::array<std::string, Scene::LightCount> LIGHT_POS_UNIFORM_NAME_ARRAY = []()
	{
		std::array<std::string, Scene::LightCount> result;
		for (size_t i = 0; i < result.size(); ++i)
		{
			result[i] = std::string("lightPos[") + std::to_string(i) + std::string("]");
		}
		return result;

	}();

	const std::array<std::string, Scene::LightCount> LIGHT_VIEW_PROJECTION_MATRIX_NAME_ARRAY = []()
	{
		std::array<std::string, Scene::LightCount> result;
		for (size_t i = 0; i < result.size(); ++i)
		{
			result[i] = std::string("lightVP[") + std::to_string(i) + std::string("]");
		}
		return result;

	}();

	const std::array<std::string, Scene::LightCount> IS_LIGHT_ACTIVE_UNFORM_NAME_ARRAY = []()
	{
		std::array<std::string, Scene::LightCount> result;
		for (size_t i = 0; i < result.size(); ++i)
		{
			result[i] = std::string("isLightActive[") + std::to_string(i) + std::string("]");
		}
		return result;

	}();

	const char* CUBE_MESH_PATH = "../../Models/cube.obj";

	const std::array<const char*,3> NormalMapPath =
	{
		"../../NormalMaps/norm1.jpg",
		"../../NormalMaps/norm2.jpg",
		"../../NormalMaps/norm3.jpg"
	};

}

void Scene::Init(const glm::ivec2& ViewPort)
{
	m_viewPort = ViewPort;
	if (!m_program.CreateShaders("shader.vert", "shader.frag"))
	{
		assert(false);
	}

	m_program.BindAttributeLocation(VertextAttribute::POSITION_ATTRIBUTE_LOCATION, VertextAttribute::POSITION_ATTRIBUTE_NAME);
	m_program.BindAttributeLocation(VertextAttribute::NORMAL_ATTRIBUTE_LOCATION, VertextAttribute::NORMAL_ATTRIBUTE_NAME);
	m_program.BindAttributeLocation(VertextAttribute::TANGENT_ATTRIBUTE_LOCATION, VertextAttribute::TANGENT_ATTRIBUTE_NAME);
	m_program.BindAttributeLocation(VertextAttribute::BITANGENT_ATTRIBUTE_LOCATION, VertextAttribute::BITANGENT_ATTRIBUTE_NAME);
	m_program.BindAttributeLocation(VertextAttribute::TEXTCOORD_ATTRIBUTE_LOCATION, VertextAttribute::TEXCOORD_ATTRIBUTE_NAME);

	m_program.LinkShaders();

	m_program.FindUniforms({
		VIEW_PROJECTION_UNIFORM_NAME,
		COLOR_UNIFORM_NAME,
		MODEL_MATRIX_UNIFORM_NAME,
		INVERSE_TRANSPOSED_MODEL_MATRIX_UNIFORM_NAME,
		CAMERA_POS_UNIFORM_NAME,
		NORMAL_MAP_UNIFORM_NAME,
		ROUGHNESS_UNIFORM_NAME
	});

	for (int i = 0; i < LightCount; ++i)
	{
		m_program.FindUniforms({
			SHADOW_MAP_UNIFORM_NAME_ARRAY[i],
			LIGHT_POS_UNIFORM_NAME_ARRAY[i],
			LIGHT_VIEW_PROJECTION_MATRIX_NAME_ARRAY[i],
			IS_LIGHT_ACTIVE_UNFORM_NAME_ARRAY[i]
		});
	}
	m_program.UseProgram();
	for (int i = 0; i < LightCount; ++i)
	{
		m_program.SetBoolUniform(true, IS_LIGHT_ACTIVE_UNFORM_NAME_ARRAY[i]);
		m_isLightActive[i] = true;
	}


	if (!m_shadowMapProgram.CreateShaders("shadowmap.vert", "shadowmap.frag"))
	{
		assert(false);
	}
	m_shadowMapProgram.BindAttributeLocation(VertextAttribute::POSITION_ATTRIBUTE_LOCATION, VertextAttribute::POSITION_ATTRIBUTE_NAME);
	m_shadowMapProgram.BindAttributeLocation(VertextAttribute::NORMAL_ATTRIBUTE_LOCATION, VertextAttribute::NORMAL_ATTRIBUTE_NAME);
	m_shadowMapProgram.BindAttributeLocation(VertextAttribute::TEXTCOORD_ATTRIBUTE_LOCATION, VertextAttribute::TEXCOORD_ATTRIBUTE_NAME);
	m_shadowMapProgram.LinkShaders();


	m_shadowMapProgram.FindUniforms({
		VIEW_PROJECTION_UNIFORM_NAME,
		MODEL_MATRIX_UNIFORM_NAME,
	});


	
	if (!m_texture3dProgramm.CreateShaders("texture3d.vert", "texture3d.frag"))
	{
		assert(false);
	}

	m_texture3dProgramm.BindAttributeLocation(VertextAttribute::POSITION_ATTRIBUTE_LOCATION, VertextAttribute::POSITION_ATTRIBUTE_NAME);
	m_texture3dProgramm.BindAttributeLocation(VertextAttribute::NORMAL_ATTRIBUTE_LOCATION, VertextAttribute::NORMAL_ATTRIBUTE_NAME);
	m_texture3dProgramm.BindAttributeLocation(VertextAttribute::TEXTCOORD_ATTRIBUTE_LOCATION, VertextAttribute::TEXCOORD_ATTRIBUTE_NAME);
	m_texture3dProgramm.LinkShaders();

	m_texture3dProgramm.FindUniforms({
		HIGHT_UNIFORM_NAME
		});



	if (!m_rockShaderProgramm.CreateShaders("rock.vert", "rock.frag"))
	{
		assert(false);
	}

	m_rockShaderProgramm.BindAttributeLocation(VertextAttribute::POSITION_ATTRIBUTE_LOCATION, VertextAttribute::POSITION_ATTRIBUTE_NAME);
	m_rockShaderProgramm.BindAttributeLocation(VertextAttribute::NORMAL_ATTRIBUTE_LOCATION, VertextAttribute::NORMAL_ATTRIBUTE_NAME);
	m_rockShaderProgramm.BindAttributeLocation(VertextAttribute::TEXTCOORD_ATTRIBUTE_LOCATION, VertextAttribute::TEXCOORD_ATTRIBUTE_NAME);
	m_rockShaderProgramm.LinkShaders();

	m_rockShaderProgramm.FindUniforms({
		VIEW_PROJECTION_UNIFORM_NAME,
		MODEL_MATRIX_UNIFORM_NAME,
		});

	m_marchingCubesShader.CreateProgram();
	m_marchingCubesShader.CreateAndAttachShader("mc.vert", ShaderType::Vertex);
	m_marchingCubesShader.CreateAndAttachShader("mc.geo", ShaderType::Geometry);
	m_marchingCubesShader.FeedBackVariings();
	m_marchingCubesShader.BindAttributeLocation(0, "in_Position");
	m_marchingCubesShader.LinkShaders();
	m_marchingCubesShader.FindUniforms({ WS_VOXEL_SIZE_UNIFORM_NAME,DENSITY_TEXTURE_UNIFORM_NAME });


	if (!m_postProcessProgram.CreateShaders("postprocess.vert", "postprocess.frag"))
	{
		assert(false);
	}
	m_postProcessProgram.BindAttributeLocation(VertextAttribute::POSITION_ATTRIBUTE_LOCATION, VertextAttribute::POSITION_ATTRIBUTE_NAME);
	m_postProcessProgram.BindAttributeLocation(VertextAttribute::NORMAL_ATTRIBUTE_LOCATION, VertextAttribute::NORMAL_ATTRIBUTE_NAME);
	m_postProcessProgram.BindAttributeLocation(VertextAttribute::TEXTCOORD_ATTRIBUTE_LOCATION, VertextAttribute::TEXCOORD_ATTRIBUTE_NAME);

	m_postProcessProgram.LinkShaders();


	m_postProcessProgram.FindUniforms({
		RENDERED_SCENE_TEXTURE_UNIFORM_NAME,
	});


	for (int i = 0; i < LightCount; ++i)
	{
		m_postProcessProgram.FindUniforms({
			SHADOW_MAP_UNIFORM_NAME_ARRAY[i],
		});
	}

	m_program.UseProgram();

	/*m_cubeShaderDatas.emplace_back(glm::vec3(0.f, 0.f, 0.f), glm::vec4(1.0f,0.f,0.f,1.f) ,0);
	m_cubeShaderDatas.emplace_back(glm::vec3(0.f, 0.f, -10.f), glm::vec4(0.0f, 1.f, 0.f, 1.f), 1);
	m_cubeShaderDatas.emplace_back(glm::vec3(0.f, -5.f, -10.f), glm::vec4(1.0f, 0.f, 1.f, 1.f), 2);
	m_cubeShaderDatas.emplace_back(glm::vec3(0.f, 5.f, 0.f), glm::vec4(1.0f, 1.f, 0.f, 1.f), 0);
	m_cubeShaderDatas.emplace_back(glm::vec3(-10.f, 0.f, -10.f), glm::vec4(0.5f, 0.5f, 0.f, 1.f), 1);
	m_cubeShaderDatas.emplace_back(glm::vec3(-10.f, -5.f, -10.f), glm::vec4(1.0f, 0.f, 0.5f, 1.f), 2);
	m_cubeShaderDatas.emplace_back(glm::vec3(0.f, 0.f, 10.f), glm::vec4(0.3f, 0.3f, 0.3f, 1.f), 0);
	m_cubeShaderDatas.emplace_back(glm::vec3(20.f, 0.f, -10.f), glm::vec4(1.0f, 1.f, 1.f, 1.f), 1);*/
	//m_cubeShaderDatas.emplace_back(glm::vec3(30.f, 0.f, -10.f), glm::vec4(0.0f, 0.f, 0.f, 1.f), 2);

	glm::vec3 size(1.f,1.f,1.f);
	m_cubeMesh.CreateInstanceOnGPU(MeshCreation::LoadFromFile(CUBE_MESH_PATH)[0]);

	m_screenTriangleMesh.CreateInstanceOnGPU(MeshCreation::CreateScreenTriangle_indexed());

	m_camera.m_position = { 40.f, 20.f, 0.f };
	m_camera.m_rotation = glm::angleAxis(0.f, glm::vec3(0, 0, 1));
	m_camera.SetPerspection(1.f, 1000.f, glm::radians(90.f), glm::vec2(ViewPort));

	

	m_spotlight[0].m_position = { 40.f, 20.f, 0.f };
	m_spotlight[0].m_direction = { -1.f, -1.f, 0.f };
	m_spotlight[0].m_direction = glm::normalize(m_spotlight[0].m_direction);
	m_spotlight[0].SetPerspection(1.f, 1000.f, 45);

	m_spotlight[1].m_position = { 20.f, 20.f, 0.f };
	m_spotlight[1].m_direction = { -1.f, -1.f, 0.f };
	m_spotlight[1].m_direction = glm::normalize(m_spotlight[0].m_direction);
	m_spotlight[1].SetPerspection(1.f, 1000.f, 45);


	for (int i = 0; i < LightCount; ++i)
	{
		m_shadowDepthTexture[i].Create();
		m_shadowDepthTexture[i].Bind();
		m_shadowDepthTexture[i].TextureImage(0, GL_DEPTH_COMPONENT, shadowDimensions.x, shadowDimensions.y, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		m_shadowDepthTexture[i].SetNearestNeighbourFiltering();
		m_shadowDepthTexture[i].SetClampBorder(glm::vec4(0.f, 0.f, 0.f, 1.f));
		m_shadowDepthTexture[i].Unbind();

		m_shadowFrameBuffer[i].Create();
		m_shadowFrameBuffer[i].Bind();
		m_shadowFrameBuffer[i].BindTexture(GL_DEPTH_ATTACHMENT, m_shadowDepthTexture[i].GetHandle(), 0);
		m_shadowFrameBuffer[i].SetDrawBuffers(GL_NONE);
		m_shadowFrameBuffer[i].SetReadBuffers(GL_NONE);
		const GLenum framBufferStatus = m_shadowFrameBuffer[i].GetFrameBufferStatus();

		assert(framBufferStatus == GL_FRAMEBUFFER_COMPLETE);
		m_shadowFrameBuffer[i].Unbind();
	}

	m_depthTexture.Create();
	m_depthTexture.Bind();
	m_depthTexture.TextureImage(0, GL_DEPTH_COMPONENT, ViewPort.x, ViewPort.y, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	m_depthTexture.SetNearestNeighbourFiltering();
	m_depthTexture.SetClampToEdge();
	m_depthTexture.Unbind();

	m_colorTexture.Create();
	m_colorTexture.Bind();
	m_colorTexture.TextureImage(0, GL_RGB, ViewPort.x, ViewPort.y, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	m_colorTexture.SetNearestNeighbourFiltering();
	m_colorTexture.SetClampToEdge();
	m_colorTexture.Unbind();

	m_framebuffer.Create();
	m_framebuffer.Bind();
	m_framebuffer.BindTexture(GL_DEPTH_ATTACHMENT, m_depthTexture.GetHandle(), 0);
	m_framebuffer.BindTexture(GL_COLOR_ATTACHMENT0, m_colorTexture.GetHandle(), 0);
	const GLenum framBufferStatus2 = m_framebuffer.GetFrameBufferStatus();
	assert(framBufferStatus2 == GL_FRAMEBUFFER_COMPLETE);
	m_framebuffer.Unbind();

	assert(NormalMapPath.size() == m_normalMaps.size());
	
	for (size_t i = 0; i < NormalMapPath.size(); ++i)
	{
		Image image;
		image.LoadImage(NormalMapPath[i]);

		assert(image.GetNumberChannels() == 3);
		m_normalMaps[i].Create();
		m_normalMaps[i].Bind();
		m_normalMaps[i].TextureImage(0, GL_RGB, image.GetWidth(), image.GetHeight(), GL_RGB, GL_UNSIGNED_BYTE, image.GetData());
		m_normalMaps[i].SetNearestNeighbourFiltering();
		m_normalMaps[i].SetClampToEdge();
		m_normalMaps[i].Unbind();
	}

	m_allowedSampleSizes = Texture2DMultisample::FindSupportedSampleSizes(m_viewPort);

	m_densityMap.Create();
	m_densityMap.Bind();
	m_densityMap.TextureImage(0, GL_R32F, Texture3dDimensions.x, Texture3dDimensions.y, Texture3dDimensions.z, GL_RED, GL_FLOAT, nullptr);
	m_densityMap.SetNearestNeighbourFiltering();
	//m_densityMap.SetClampToEdge();
	m_densityMap.Unbind();



	// create dummy attributeBuffer
	{
		std::vector<glm::vec3> dummyPoints;
		for (int x = 1; x <= Texture3dDimensionsMinOne.x; ++x)
		{
			for (int y = 1; y <= Texture3dDimensionsMinOne.y; ++y)
			{
				for (int z = 1; z <= Texture3dDimensionsMinOne.z; ++z)
				{
					dummyPoints.push_back(glm::vec3(
						x * Inversed3dDimensions.x,
						y * Inversed3dDimensions.y,
						z * Inversed3dDimensions.z));
				}
			}
		}

		m_marchingCubesVao.Create();
		m_marchingCubesVao.Bind();
		m_marchingCubesVao.EnableAttribute(0);

		m_dummyVertices.Create();
		m_dummyVertices.Bind();
		m_dummyVertices.UploadBufferData(dummyPoints);
		m_dummyVertices.SetVertexAttributePtr(0, glm::vec3::length(), GL_FLOAT, sizeof(glm::vec3), 0);
		m_dummyVertices.Unbind();

		m_marchingCubesVao.Unbind();
	}

	m_mcLookupBuffer.WriteLookupTablesToGpu();

	DensityPass();
	GenerateRockFromDensity();

	m_rockVao.Create();
	m_rockVao.Bind();
	m_rockVao.EnableAttribute(0);

	m_marchingCubesVao.Unbind();

}

void Scene::Update(float deltaTime, const InputManager& inputManager)
{
	if (m_path.IsFollowingPath())
	{
		UpdatePathFollowing(deltaTime, inputManager);
	}
	else
	{
		m_pathFollower = PathFollower::none;
	}
	
	if(!m_path.IsFollowingPath() || m_pathFollower != PathFollower::camera)
	{
		UpdateFreeMovement(deltaTime, inputManager);
	}
}


void Scene::UpdateFreeMovement(float deltaTime, const InputManager & inputManager)
{
	const glm::vec2 mousePosition = inputManager.GetMousePosition();
	glm::vec3 euler(mousePosition.y *0.01, mousePosition.x *0.01f, 0);

	m_camera.m_rotation = glm::dquat(euler);

	glm::vec3 forwardVector = glm::mat3_cast(m_camera.m_rotation) * glm::vec3(0, 0, 1);
	glm::vec3 leftVector = glm::mat3_cast(m_camera.m_rotation) * glm::vec3(1, 0, 0);

	if (inputManager.GetKey(KeyCode::KEY_W).IsPressed())
	{
		m_camera.m_position += deltaTime * forwardVector * 10.f;
	}
	if (inputManager.GetKey(KeyCode::KEY_S).IsPressed())
	{
		m_camera.m_position -= deltaTime * forwardVector * 10.f;
	}

	if (inputManager.GetKey(KeyCode::KEY_A).IsPressed())
	{
		m_camera.m_position += deltaTime * leftVector * 10.f;
	}
	if (inputManager.GetKey(KeyCode::KEY_D).IsPressed())
	{
		m_camera.m_position -= deltaTime * leftVector * 10.f;
	}

	if (inputManager.GetKey(KeyCode::KEY_1).GetNumPressed() > 0)
	{
		m_path.AddKeyFrame(m_camera.m_rotation, m_camera.m_position);
	}

	if (inputManager.GetKey(KeyCode::KEY_2).GetNumPressed() > 0 && m_path.HasKeyFrames())
	{
		KeyFrame removedFrame = m_path.RemoveLastKeyFrame();
		m_camera.m_position = removedFrame.position;
		m_camera.m_rotation = removedFrame.rotation;
	}

	if (inputManager.GetKey(KeyCode::KEY_3).GetNumPressed() > 0)
	{
		m_isLightActive[0] = !m_isLightActive[0];
		m_program.UseProgram();
		m_program.SetBoolUniform(m_isLightActive[0], IS_LIGHT_ACTIVE_UNFORM_NAME_ARRAY[0]);
	}

	if (inputManager.GetKey(KeyCode::KEY_4).GetNumPressed() > 0)
	{
		m_isLightActive[1] = !m_isLightActive[1];
		m_program.UseProgram();
		m_program.SetBoolUniform(m_isLightActive[1], IS_LIGHT_ACTIVE_UNFORM_NAME_ARRAY[1]);
	}

	if (inputManager.GetKey(KeyCode::KEY_C).GetNumPressed() > 0)
	{
		if (m_pathFollower == PathFollower::none)
		{
			m_pathFollower = PathFollower::camera;
			m_path.TryStartPathFollowing();
		}
	}

	if (inputManager.GetKey(KeyCode::KEY_L).GetNumPressed() > 0)
	{
		if (m_pathFollower == PathFollower::none)
		{
			m_pathFollower = PathFollower::light0;
			m_path.TryStartPathFollowing();
		}
	}

	if (inputManager.GetKey(KeyCode::KEY_K).GetNumPressed() > 0)
	{
		if (m_pathFollower == PathFollower::none)
		{
			m_pathFollower = PathFollower::light1;
			m_path.TryStartPathFollowing();
		}
	}

	if (inputManager.GetKey(KeyCode::PLUS).IsPressed())
	{
		m_currentRoughness = std::min(1.0f, m_currentRoughness + 0.05f);
	}
	if (inputManager.GetKey(KeyCode::MINUS).IsPressed())
	{
		m_currentRoughness = std::max(0.0f, m_currentRoughness - 0.05f);
	}

	if (inputManager.GetKey(KeyCode::KEY_E).GetNumPressed() > 0)
	{
		++m_sampleIndex;
		if (m_sampleIndex >= m_allowedSampleSizes.size())
		{
			m_sampleIndex = 0;
		}

		SetMsaaSamplingNumber(m_allowedSampleSizes[m_sampleIndex], m_msaaFixedSamples);
	}
	if (inputManager.GetKey(KeyCode::KEY_F).GetNumPressed() > 0)
	{
		SetMsaaSamplingNumber(m_allowedSampleSizes[m_sampleIndex], !m_msaaFixedSamples);
	}
}

void Scene::UpdatePathFollowing(float deltaTime, const InputManager& inputManager)
{
	if (inputManager.GetKey(KeyCode::KEY_SPACE).GetNumPressed() > 0)
	{
		m_path.StopPathFollowing();
		m_pathFollower = PathFollower::none;
		return;
	}

	switch (m_pathFollower)
	{
	case PathFollower::camera:
		m_path.UpdatePathFollowing(deltaTime, inputManager, m_camera);
		break;
	case PathFollower::light0:
		m_path.UpdatePathFollowing(deltaTime, m_spotlight[0]);
		break;
	case PathFollower::light1:
		m_path.UpdatePathFollowing(deltaTime, m_spotlight[1]);
		break;
	default:
		assert(false);
	}
}

void Scene::Render()
{
	for (int i = 0; i < LightCount; ++i)
	{
		if(m_isLightActive[i]) // save performance
		{
			ShadowMapPass(i);
		}
	}
	RenderScenePass();
	RenderRock();
	PostProcessPass();
}

void Scene::SetMsaaSamplingNumber(GLsizei samples, bool fixedSampleLocations)
{
	m_msaaFixedSamples = fixedSampleLocations;
	if (samples < 2)
	{
		return;
	}

	if (!m_msaaColorTexture.IsValid())
	{
		m_msaaColorTexture.Create();
	}
	m_msaaColorTexture.Bind();
	m_msaaColorTexture.TexImage2DMultisample(samples, GL_RGB, m_viewPort, fixedSampleLocations);
	m_msaaColorTexture.Unbind();

	if (!m_msaaDepthTexture.IsValid())
	{
		m_msaaDepthTexture.Create();
	}
	m_msaaDepthTexture.Bind();
	m_msaaDepthTexture.TexImage2DMultisample(samples, GL_DEPTH_COMPONENT, m_viewPort, fixedSampleLocations);
	m_msaaDepthTexture.Unbind();


	if (!m_msaaFrameBuffer.IsValid())
	{
		m_msaaFrameBuffer.Create();
	}
	m_msaaFrameBuffer.Bind();
	m_msaaFrameBuffer.BindMultisampleTexture(GL_COLOR_ATTACHMENT0, m_msaaColorTexture.GetHandle());
	m_msaaFrameBuffer.BindMultisampleTexture(GL_DEPTH_ATTACHMENT, m_msaaDepthTexture.GetHandle());

	const GLenum framBufferStatus2 = m_msaaFrameBuffer.GetFrameBufferStatus();
	assert(framBufferStatus2 == GL_FRAMEBUFFER_COMPLETE);
	assert(m_msaaFrameBuffer.IsValid());

	m_msaaFrameBuffer.Unbind();

	assert(m_msaaColorTexture.IsValid());
	assert(m_msaaDepthTexture.IsValid());
	assert(m_msaaFrameBuffer.IsValid());
}

void Scene::DensityPass()
{
	m_densityFramebuffer.Create();
	m_densityFramebuffer.Bind();

	glViewport(0, 0, Texture3dDimensions.x, Texture3dDimensions.y);

	m_texture3dProgramm.UseProgram();
	m_texture3dProgramm.IsValid();
	
	for (int layer = 0; layer < Texture3dDimensions.z; ++layer)
	{
		m_densityFramebuffer.BindTexture3D(GL_COLOR_ATTACHMENT0, m_densityMap.GetHandle(), 0, layer);
	
		m_texture3dProgramm.SetFloatUniform(layer * Texture3dSliceHeight, HIGHT_UNIFORM_NAME);
		const GLenum framBufferStatus2 = m_densityFramebuffer.GetFrameBufferStatus();
		assert(framBufferStatus2 == GL_FRAMEBUFFER_COMPLETE);
		glClear(GL_COLOR_BUFFER_BIT);
		ASSERT_GL_ERROR_MACRO();

		m_screenTriangleMesh.Render();
	}

	m_rockVertices.Create();
	m_rockVertices.Bind();
	//m_rockVertices.AllocateBufferData(Texture3dNumSamples * sizeof(glm::vec3), GL_STATIC_READ);
	m_rockVertices.AllocateBufferData(Texture3dNumSamples * sizeof(glm::vec3) * 3, GL_STATIC_READ);

	m_rockVertices.Unbind();
	m_densityFramebuffer.Unbind();


}

void Scene::GenerateRockFromDensity()
{

	glEnable(GL_RASTERIZER_DISCARD);
	{
		m_marchingCubesShader.UseProgram();
		m_marchingCubesShader.SetFloatUniform(Texture3dSliceHeight, WS_VOXEL_SIZE_UNIFORM_NAME);

		m_mcLookupBuffer.UpdateUniforms(m_marchingCubesShader);

		m_marchingCubesVao.Bind();

		glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_rockVertices.GetHandle());

		m_marchingCubesShader.SetSamplerTextureUnit(0, DENSITY_TEXTURE_UNIFORM_NAME);
		m_densityMap.BindToTextureUnit(0);

		GLuint query;
		glGenQueries(1, &query);
		glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, query);
		ASSERT_GL_ERROR_MACRO();

		glBeginTransformFeedback(GL_TRIANGLES);
		ASSERT_GL_ERROR_MACRO();

		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDrawArrays(GL_POINTS, 0, Texture3dNumSamples);
		ASSERT_GL_ERROR_MACRO();


		glEndTransformFeedback();
		ASSERT_GL_ERROR_MACRO();

		glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
		ASSERT_GL_ERROR_MACRO();

	
		glGetQueryObjectuiv(query, GL_QUERY_RESULT, &m_numRockTriangles);
		ASSERT_GL_ERROR_MACRO();

		printf("%u primitives written!\n\n", m_numRockTriangles);

		m_marchingCubesVao.Unbind();
	}
	glDisable(GL_RASTERIZER_DISCARD);
	glFlush();

	/*GLfloat*  feedback=  new GLfloat[m_numRockTriangles];
	glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, sizeof(float) * m_numRockTriangles, feedback);
	for (int i = 0; i <m_numRockTriangles; ++i)
	{
		printf("%f ", feedback[i]);
	}
	delete[] feedback;*/


}

void Scene::RenderRock()
{
	glViewport(0, 0, m_viewPort.x, m_viewPort.y);
	m_framebuffer.Bind();
	m_rockVao.Bind();
	m_rockVao.EnableAttribute(0);
	m_rockVertices.Bind();
	m_rockVertices.SetVertexAttributePtr(0, glm::vec3::length(), GL_FLOAT, sizeof(glm::vec3), 0);
	glDrawArrays(GL_TRIANGLES, 0, m_numRockTriangles * 3);

	m_rockVao.Unbind();
	m_framebuffer.Unbind();
}

void Scene::ShadowMapPass(int LightIndex)
{
	assert(LightIndex < LightCount);
	glCullFace(GL_FRONT);
	glViewport(0, 0, shadowDimensions.x, shadowDimensions.y);
	m_shadowFrameBuffer[LightIndex].Bind();
	m_shadowMapProgram.UseProgram();
	m_shadowFrameBuffer[LightIndex].BindTexture(GL_DEPTH_ATTACHMENT, m_shadowDepthTexture[LightIndex].GetHandle(), 0);

	glClear(GL_DEPTH_BUFFER_BIT);
	ASSERT_GL_ERROR_MACRO();

	m_shadowMapProgram.IsValid();
	glm::mat4 viewProjection = m_spotlight[LightIndex].GetPerspectionMatrix() * m_spotlight[LightIndex].CalcViewMatrix();
	m_shadowMapProgram.SetMatrixUniform(viewProjection, VIEW_PROJECTION_UNIFORM_NAME);

	m_shadowMapProgram.SetMatrixUniform(glm::translate(glm::scale(glm::vec3{ 50.f,1.f,50.f }), glm::vec3(0.f, -10.f, 0.f)), MODEL_MATRIX_UNIFORM_NAME);
	m_cubeMesh.Render();

	for (const auto& cubeData : m_cubeShaderDatas)
	{
		m_shadowMapProgram.SetMatrixUniform(glm::translate(glm::mat4(1.0), cubeData.position), MODEL_MATRIX_UNIFORM_NAME);
		m_cubeMesh.Render();
	}
	m_shadowFrameBuffer[LightIndex].Unbind();
	glCullFace(GL_BACK);
}

void Scene::RenderScenePass()
{
	if (m_allowedSampleSizes[m_sampleIndex] > 1)
	{
		m_msaaFrameBuffer.Bind();
		m_msaaFrameBuffer.BindMultisampleTexture(GL_DEPTH_ATTACHMENT, m_msaaDepthTexture.GetHandle());
		m_msaaFrameBuffer.BindMultisampleTexture(GL_COLOR_ATTACHMENT0, m_msaaColorTexture.GetHandle());
	}
	else
	{
		m_framebuffer.Bind();
		m_framebuffer.BindTexture(GL_DEPTH_ATTACHMENT, m_depthTexture.GetHandle(), 0);
		m_framebuffer.BindTexture(GL_COLOR_ATTACHMENT0, m_colorTexture.GetHandle(), 0);
	}

	
	m_program.UseProgram();
	
	glViewport(0, 0, m_viewPort.x, m_viewPort.y);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ASSERT_GL_ERROR_MACRO();

	int textureUnitCnt = 0;

	for (int i = 0; i < LightCount; ++i)
	{
		m_program.SetSamplerTextureUnit(textureUnitCnt, SHADOW_MAP_UNIFORM_NAME_ARRAY[i]);
		m_shadowDepthTexture[i].BindToTextureUnit(textureUnitCnt);
		++textureUnitCnt;
	}
	

	glm::mat4 viewProjection = m_camera.GetPerspectionMatrix() * m_camera.CalcViewMatrix();

	m_program.SetMatrixUniform(viewProjection, VIEW_PROJECTION_UNIFORM_NAME);

	for (int i = 0; i < LightCount; ++i)
	{
		glm::mat4 lightViewProjection = m_spotlight[i].GetPerspectionMatrix() * m_spotlight[i].CalcViewMatrix();
		m_program.SetMatrixUniform(lightViewProjection, LIGHT_VIEW_PROJECTION_MATRIX_NAME_ARRAY[i]);
		m_program.SetVec3Uniform(m_spotlight[i].m_position, LIGHT_POS_UNIFORM_NAME_ARRAY[i]);
	}	

	m_program.SetVec3Uniform(m_camera.m_position, CAMERA_POS_UNIFORM_NAME);

	m_program.IsValid();

	const int normMapTextureUnit = textureUnitCnt;
	++textureUnitCnt;

	// draw light source
	for (int i = 0; i < LightCount; ++i)
	{
		glm::mat4 lightModelMatrix = glm::scale(glm::translate(m_spotlight[i].m_position), glm::vec3(1.f, 1.f, 1.1f));
		m_program.SetMatrixUniform(lightModelMatrix, MODEL_MATRIX_UNIFORM_NAME);
		m_program.SetMatrixUniform(glm::transpose(glm::inverse(lightModelMatrix)), INVERSE_TRANSPOSED_MODEL_MATRIX_UNIFORM_NAME);
		m_program.SetVec4Uniform(glm::vec4(1.f, 1.f, 0.0f, 1.f), COLOR_UNIFORM_NAME);
		m_cubeMesh.Render();
	}
	m_program.SetFloatUniform(0.0f, ROUGHNESS_UNIFORM_NAME);
	// Plattform
	{
		m_program.SetSamplerTextureUnit(normMapTextureUnit, NORMAL_MAP_UNIFORM_NAME);
		m_normalMaps[0].BindToTextureUnit(normMapTextureUnit);

		glm::mat4 plattformModelMat = glm::translate(glm::vec3(0.f, -10.f, 0.f));
		plattformModelMat = glm::scale(plattformModelMat, glm::vec3{ 50.f,1.f,50.f });
		m_program.SetMatrixUniform(plattformModelMat, MODEL_MATRIX_UNIFORM_NAME);
		m_program.SetMatrixUniform(glm::transpose(glm::inverse(plattformModelMat)), INVERSE_TRANSPOSED_MODEL_MATRIX_UNIFORM_NAME);

		m_program.SetVec4Uniform(glm::vec4(0.2f, 0.8f, 0.8f, 1.f), COLOR_UNIFORM_NAME);
		m_cubeMesh.Render();
	}

	assert(0.f <= m_currentRoughness && m_currentRoughness <= 1.0f);

	m_program.SetFloatUniform(m_currentRoughness, ROUGHNESS_UNIFORM_NAME);
	for (const auto& cubeData : m_cubeShaderDatas)
	{
		glm::mat4 modelMatrix = glm::translate(cubeData.position);
		//modelMatrix = glm::scale(modelMatrix, glm::vec3(50.f, 50.f, 50.f));

		assert(cubeData.normalMapId < m_normalMaps.size());
		m_program.SetSamplerTextureUnit(normMapTextureUnit, NORMAL_MAP_UNIFORM_NAME);
		m_normalMaps[cubeData.normalMapId].BindToTextureUnit(normMapTextureUnit);

		m_program.SetMatrixUniform(modelMatrix, MODEL_MATRIX_UNIFORM_NAME);
		m_program.SetMatrixUniform(glm::transpose(glm::inverse(modelMatrix)), INVERSE_TRANSPOSED_MODEL_MATRIX_UNIFORM_NAME);
		
		m_program.SetVec4Uniform(cubeData.color, COLOR_UNIFORM_NAME);
		m_cubeMesh.Render();
	}
	m_program.SetFloatUniform(0.0f, ROUGHNESS_UNIFORM_NAME);


	for (const auto& cubeData : m_path.GetPathCubes())
	{
		glm::mat4 modelMatrix = glm::scale(glm::translate(cubeData.position), glm::vec3(.1f, .1f, .1f));
		m_program.SetMatrixUniform(modelMatrix, MODEL_MATRIX_UNIFORM_NAME);
		m_program.SetMatrixUniform(glm::transpose(glm::inverse(modelMatrix)), INVERSE_TRANSPOSED_MODEL_MATRIX_UNIFORM_NAME);

		m_program.SetVec4Uniform(cubeData.color, COLOR_UNIFORM_NAME);
		m_cubeMesh.Render();
	}

	if (m_allowedSampleSizes[m_sampleIndex] > 1)
	{
		FrameBufferUtils::CopyBufferData(m_msaaFrameBuffer, m_framebuffer, m_viewPort);
	}

	m_msaaFrameBuffer.Unbind();
	m_framebuffer.Unbind();
}

void Scene::PostProcessPass()
{
	glViewport(0, 0, m_viewPort.x, m_viewPort.y);
	
	m_postProcessProgram.UseProgram();

	m_postProcessProgram.SetSamplerTextureUnit(0, RENDERED_SCENE_TEXTURE_UNIFORM_NAME);
	m_colorTexture.BindToTextureUnit(0);

	for (int i = 0; i < LightCount; ++i)
	{
		m_postProcessProgram.SetSamplerTextureUnit(i+1 , SHADOW_MAP_UNIFORM_NAME_ARRAY[i]);
		m_shadowDepthTexture[i].BindToTextureUnit(i+1);
	}

	m_postProcessProgram.IsValid();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ASSERT_GL_ERROR_MACRO();

	m_screenTriangleMesh.Render();

}



