#include "stdafx.h"
#include "Scene.h"
#include "InputManager.h"
#include "gtx/spline.hpp"
#include "gtx/quaternion.hpp"
#include <string>
#include "Image.h"
#include "KdTreeTraverser.h"
#include "LineMesh.h"
namespace
{
	const glm::mat4 rockModelMat = glm::rotate(glm::radians(-90.f), glm::vec3(1.f, 0.f, 0.f)) * glm::scale(glm::vec3(5.f, 5.f, 20.f));

	const glm::ivec2 shadowDimensions(1024, 1024);

	const std::string ViewProjectionUniformName("viewProjection");
	const std::string COLOR_UNIFORM_NAME("color");
	const std::string MODEL_MATRIX_UNIFORM_NAME("model");
	const std::string RENDERED_SCENE_TEXTURE_UNIFORM_NAME("renderedscene");
	const std::string INVERSE_TRANSPOSED_MODEL_MATRIX_UNIFORM_NAME("inverseTransposedModelMat");
	const std::string CAMERA_POS_UNIFORM_NAME("cameraPos");
	const std::string NORMAL_MAP_UNIFORM_NAME("normalMap");
	const std::string ROUGHNESS_UNIFORM_NAME("roughness");

	const std::string COLOR_TEX_UNIFORM_NAME("colorTex");
	const std::string NORMAL_TEX_UNIFORM_NAME("normalTex");
	const std::string DISPLACEMENT_TEX_UNIFORM_NAME("dispTex");

	const std::string DISPLACEMENT_LAYERS_UNIFORM_NAME("numLayers");
	const std::string DISPLACEMENT_REFINEMENT_LAYERS_UNIFORM_NAME("numRefinementLayers");
	const std::string DISPLACEMENT_HEIGHT_UNIFORM_NAME("dispHeight");
	const std::string USE_ESM_UNIFORM_NAME("useEsm");
	const std::string ESM_MODIFIER_UNIFORM_NAME("esmModifier");





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
	const char* TORUS_MESH_PATH = "../../Models/torus.obj";
	const char* CONE_MESH_PATH = "../../Models/cone.obj";


	const std::array<const char*, 3> NormalMapPath =
	{
		"../../NormalMaps/norm1.jpg",
		"../../NormalMaps/norm2.jpg",
		"../../NormalMaps/norm3.jpg"
	};

	glm::vec2 normalizeMousePosition(glm::vec2 mousePos, glm::vec2 viewportSize) {
		glm::vec2 norm1 = glm::vec2(mousePos.x / viewportSize.x, mousePos.y / viewportSize.y);
		return (norm1 * 2) - glm::vec2(1, 1);
	}


	glm::vec3 screenToWorld(glm::vec3 screenpos, const Camera& camera)
	{
		glm::vec4 cameraSpace = glm::inverse(camera.GetPerspectionMatrix()) * glm::vec4(screenpos, 1.f);
		cameraSpace /= cameraSpace.w;
		glm::vec4 worldSpace = glm::inverse(camera.CalcViewMatrix()) * cameraSpace;
		return glm::vec3(worldSpace);
	}

}

void Scene::Init(const glm::ivec2& ViewPort)
{
	m_gausBlur = 1;
	m_esmModifier = 80.f;
	m_useEsm = true;
	m_viewPort = ViewPort;

	if (!m_rockShaderProgram.CreateShaders("../Shader/rock.vert", "../Shader/rock.frag"))
	{
		assert(false);
	}
	m_rockShaderProgram.BindAttributeLocation(ProceduralMeshVertex::PositionLocation, ProceduralMeshVertex::POSITION_ATTRIBUTE_NAME);
	m_rockShaderProgram.BindAttributeLocation(ProceduralMeshVertex::NormalLocation, ProceduralMeshVertex::NORMAL_ATTRIBUTE_NAME);

	m_rockShaderProgram.LinkShaders();
	m_rockShaderProgram.FindUniforms({
		ViewProjectionUniformName,
		MODEL_MATRIX_UNIFORM_NAME,
		INVERSE_TRANSPOSED_MODEL_MATRIX_UNIFORM_NAME,
		CAMERA_POS_UNIFORM_NAME,
		COLOR_TEX_UNIFORM_NAME,
		NORMAL_TEX_UNIFORM_NAME,
		DISPLACEMENT_TEX_UNIFORM_NAME,
		DISPLACEMENT_LAYERS_UNIFORM_NAME,
		DISPLACEMENT_REFINEMENT_LAYERS_UNIFORM_NAME,
		DISPLACEMENT_HEIGHT_UNIFORM_NAME,
		});

	for (int i = 0; i < LightCount; ++i)
	{
		m_rockShaderProgram.FindUniform(LIGHT_POS_UNIFORM_NAME_ARRAY[i]);
	}


	const auto setupRockTexture = [](Texture& tex, const char* path)
	{
		Image image;
		image.LoadImage(path);
		assert(image.GetNumberChannels() == 3);

		tex.Create();
		tex.Bind();
		tex.TextureImage(0, GL_RGB, image.GetWidth(), image.GetHeight(), GL_RGB, GL_UNSIGNED_BYTE, image.GetData());
		tex.SetLinearFiltering();
		tex.SetRepeating();
		tex.Unbind();
	};

	setupRockTexture(m_rockColor, "../Textures/Stone_02/color.jpg");
	setupRockTexture(m_rockDisp, "../Textures/Stone_02/disp.jpg");
	setupRockTexture(m_rockNormal, "../Textures/Stone_02/normal.jpg");



	if (!m_program.CreateShaders("../Shader/shader.vert", "../Shader/shader.frag"))
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
		ViewProjectionUniformName,
		COLOR_UNIFORM_NAME,
		MODEL_MATRIX_UNIFORM_NAME,
		INVERSE_TRANSPOSED_MODEL_MATRIX_UNIFORM_NAME,
		CAMERA_POS_UNIFORM_NAME,
		NORMAL_MAP_UNIFORM_NAME,
		ROUGHNESS_UNIFORM_NAME,
		USE_ESM_UNIFORM_NAME,
		ESM_MODIFIER_UNIFORM_NAME
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


	if (!m_shadowMapProgram.CreateShaders("../Shader/shadowmap.vert", "../Shader/shadowmap.frag"))
	{
		assert(false);
	}
	m_shadowMapProgram.BindAttributeLocation(VertextAttribute::POSITION_ATTRIBUTE_LOCATION, VertextAttribute::POSITION_ATTRIBUTE_NAME);
	m_shadowMapProgram.BindAttributeLocation(VertextAttribute::NORMAL_ATTRIBUTE_LOCATION, VertextAttribute::NORMAL_ATTRIBUTE_NAME);
	m_shadowMapProgram.BindAttributeLocation(VertextAttribute::TEXTCOORD_ATTRIBUTE_LOCATION, VertextAttribute::TEXCOORD_ATTRIBUTE_NAME);
	m_shadowMapProgram.LinkShaders();


	m_shadowMapProgram.FindUniforms({
		ViewProjectionUniformName,
		MODEL_MATRIX_UNIFORM_NAME,
		});


	if (!m_postProcessProgram.CreateShaders("../Shader/postprocess.vert", "../Shader/postprocess.frag"))
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
	m_program.SetBoolUniform(m_useEsm, USE_ESM_UNIFORM_NAME);
	m_program.SetFloatUniform(m_esmModifier, ESM_MODIFIER_UNIFORM_NAME);
	m_cubeShaderDatas.emplace_back(glm::vec3(0.f, 0.f, 0.f), glm::vec4(1.0f, 0.f, 0.f, 1.f), 0);
	m_cubeShaderDatas.emplace_back(glm::vec3(0.f, 0.f, -10.f), glm::vec4(0.0f, 1.f, 0.f, 1.f), 1);
	m_cubeShaderDatas.emplace_back(glm::vec3(0.f, -5.f, -10.f), glm::vec4(1.0f, 0.f, 1.f, 1.f), 2);
	m_cubeShaderDatas.emplace_back(glm::vec3(0.f, 5.f, 0.f), glm::vec4(1.0f, 1.f, 0.f, 1.f), 0);
	m_cubeShaderDatas.emplace_back(glm::vec3(-10.f, 0.f, -10.f), glm::vec4(0.5f, 0.5f, 0.f, 1.f), 1);
	m_cubeShaderDatas.emplace_back(glm::vec3(-10.f, -5.f, -10.f), glm::vec4(1.0f, 0.f, 0.5f, 1.f), 2);
	m_cubeShaderDatas.emplace_back(glm::vec3(0.f, 0.f, 10.f), glm::vec4(0.3f, 0.3f, 0.3f, 1.f), 0);
	m_cubeShaderDatas.emplace_back(glm::vec3(20.f, 0.f, -10.f), glm::vec4(1.0f, 1.f, 1.f, 1.f), 1);
	m_cubeShaderDatas.emplace_back(glm::vec3(30.f, 0.f, -10.f), glm::vec4(0.0f, 0.f, 0.f, 1.f), 2);
	m_cubeShaderDatas.emplace_back(glm::vec3(0.f, 50.f, 0.f), glm::vec4(0.0f, 1.f, 1.f, 1.f), 2);


	glm::vec3 size(1.f, 1.f, 1.f);
	const auto CubeVertexData = MeshCreation::LoadFromFile(CUBE_MESH_PATH)[0];
	m_cubeMesh.CreateInstanceOnGPU(CubeVertexData);
	const auto TorusVertexData = MeshCreation::LoadFromFile(TORUS_MESH_PATH)[0];
	const auto ConeVertexData = MeshCreation::LoadFromFile(CONE_MESH_PATH)[0];


	m_screenTriangleMesh.CreateInstanceOnGPU(MeshCreation::CreateScreenTriangle_indexed());

	m_camera.m_position = { 40.f, 20.f, 0.f };
	m_camera.m_rotation = glm::angleAxis(0.f, glm::vec3(0, 0, 1));
	m_camera.SetPerspection(0.01f, 1000.f, glm::radians(90.f), glm::vec2(ViewPort));



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
	m_mcLookup.WriteLookupTablesToGpu();
	m_rock.Init();
	m_rock.GenerateMesh(m_mcLookup,0.f);
	//m_rock.GeneratedKdTree(rockModelMat);

	{
		m_particleSystem[0].Init(
			10'000,
			"../Shader/particleUpdate.vert", "../Shader/particleUpdate.geo",
			"../Shader/particleDraw.vert", "../Shader/particleDraw.frag"
		);
		m_particleSystem[0].SetParticelMesh(CubeVertexData.data(), CubeVertexData.size());
	}
	{
		m_particleSystem[1].Init(
			10'000,
			"../Shader/particleUpdate1.vert", "../Shader/particleUpdate1.geo",
			"../Shader/particleDraw1.vert", "../Shader/particleDraw1.frag"
		);
		m_particleSystem[1].SetParticelMesh(TorusVertexData.data(), TorusVertexData.size());
	}
	{
		m_particleSystem[2].Init(
			10'000,
			"../Shader/particleUpdate2.vert", "../Shader/particleUpdate2.geo",
			"../Shader/particleDraw2.vert", "../Shader/particleDraw2.frag"
		);
		m_particleSystem[2].SetParticelMesh(ConeVertexData.data(), ConeVertexData.size());
	}

	if (!m_lineShaderProgram.CreateShaders("../Shader/lineshader.vert", "../Shader/lineshader.frag"))
	{
		assert(false);
	}

	m_lineShaderProgram.LinkShaders();

	m_lineShaderProgram.FindUniforms({
		ViewProjectionUniformName,
		COLOR_UNIFORM_NAME
		});

	m_esmDepthTexture.Create();
	m_esmDepthTexture.Bind();
	m_esmDepthTexture.TextureImage(0, GL_DEPTH_COMPONENT, shadowDimensions.x, shadowDimensions.y, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	m_esmDepthTexture.SetNearestNeighbourFiltering();
	m_esmDepthTexture.SetClampToEdge();
	m_esmDepthTexture.Unbind();
	
	m_esmShadowFrameBuffer.Create();
	m_esmShadowFrameBuffer.Bind();
	m_esmShadowFrameBuffer.BindTexture(GL_DEPTH_ATTACHMENT, m_esmDepthTexture.GetHandle(), 0);
	m_esmShadowFrameBuffer.Unbind();
	
	for (Texture& tex : m_esmShadowDepthTextures)
	{
		tex.Create();
		tex.Bind();
		tex.TextureImage(0, GL_R32F, shadowDimensions.x, shadowDimensions.y, GL_RED, GL_FLOAT, nullptr);
		tex.SetNearestNeighbourFiltering();
		tex.SetClampBorder(glm::vec4(0.f, 0.f, 0.f, 1.f));
		tex.Unbind();
	}

	
	m_esmShadowDepthTemporaryTexture.Create();
	m_esmShadowDepthTemporaryTexture.Bind();
	m_esmShadowDepthTemporaryTexture.TextureImage(0, GL_R32F, shadowDimensions.x, shadowDimensions.y, GL_RED, GL_FLOAT, nullptr);
	m_esmShadowDepthTemporaryTexture.SetNearestNeighbourFiltering();
	m_esmShadowDepthTemporaryTexture.SetClampBorder(glm::vec4(0.f, 0.f, 0.f, 1.f));
	m_esmShadowDepthTemporaryTexture.Unbind();

	if (!m_esmShadowMapProgram.CreateShaders("../Shader/esmShadowmap.vert", "../Shader/esmShadowmap.frag"))
	{
		assert(false);
	}
	m_esmShadowMapProgram.BindAttributeLocation(VertextAttribute::POSITION_ATTRIBUTE_LOCATION, VertextAttribute::POSITION_ATTRIBUTE_NAME);
	m_esmShadowMapProgram.BindAttributeLocation(VertextAttribute::NORMAL_ATTRIBUTE_LOCATION, VertextAttribute::NORMAL_ATTRIBUTE_NAME);
	m_esmShadowMapProgram.BindAttributeLocation(VertextAttribute::TEXTCOORD_ATTRIBUTE_LOCATION, VertextAttribute::TEXCOORD_ATTRIBUTE_NAME);
	m_esmShadowMapProgram.LinkShaders();
	m_esmShadowMapProgram.FindUniforms({ ViewProjectionUniformName, MODEL_MATRIX_UNIFORM_NAME, ESM_MODIFIER_UNIFORM_NAME });
	m_gausFilterer.Init();

	m_terrain.Init(glm::vec2(100.f,100.f),glm::vec3(160.f,-30.f,0.f), 20.f,"../Textures/Stone_02/disp.jpg", "../Textures/Stone_02/color.jpg");
}


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


void Scene::Update(float deltaTime, const InputManager& inputManager)
{
	if (m_printFramerate)
	{
		printf("framerate is now: %f\n",  1.f / deltaTime);
	}
	for (auto& ps : m_particleSystem)
	{
		ps.Update(deltaTime * m_particleUpdateRateModifier);
	}

	if (inputManager.GetKey(KeyCode::KEY_COMMA).GetNumPressed() > 0)
	{
		m_useWireframe = !m_useWireframe;
		printf("wireframe is now %s \n", m_useWireframe ? "active" : "inactive");
	}
	

	if (inputManager.GetKey(KeyCode::KEY_B).GetNumPressed() > 0)
	{
		m_terrain.m_maxHeight *= 1.3f;
		printf("terrain max height is now: %f\n", m_terrain.m_maxHeight);
	}

	if (inputManager.GetKey(KeyCode::KEY_V).GetNumPressed() > 0)
	{
		m_terrain.m_maxHeight /= 1.3f;
		printf("terrain max height is now: %f\n", m_terrain.m_maxHeight);
	}

	if (inputManager.GetKey(KeyCode::KEY_N).GetNumPressed() > 0)
	{
		m_terrain.m_quality *= 2.f;
		printf("terrain quality is now: %f\n", m_terrain.m_quality);
	}

	if (inputManager.GetKey(KeyCode::KEY_M).GetNumPressed() > 0)
	{
		m_terrain.m_quality = m_terrain.m_quality / 2.f;
		printf("terrain quality is now: %f\n", m_terrain.m_quality);
	}

	if (inputManager.GetKey(KeyCode::KEY_T).GetNumPressed() > 0)


	if (inputManager.GetKey(KeyCode::KEY_T).GetNumPressed() > 0)
	{
		m_printFramerate = !m_printFramerate;
	}

	if (inputManager.GetKey(KeyCode::KEY_X).GetNumPressed() > 0)
	{

		m_esmModifier *= 1.1f;// = std::min(20.f, m_esmModifier + 0.2f);
		printf("esmModifier is now: %f\n", m_esmModifier);
	}
	if (inputManager.GetKey(KeyCode::KEY_Y).GetNumPressed() > 0)
	{
		m_esmModifier /= 1.1f;// std::max(5.f, m_esmModifier - 0.2f);
		printf("esmModifier is now: %f\n", m_esmModifier);
	}

	if (inputManager.GetKey(KeyCode::KEY_G).GetNumPressed() > 0)
	{
		m_gausBlur *= 2;
		// base radius is 3, is defined in shader
		printf("Gaus Radius is now is now: %i\n", m_gausBlur * 3);
	}
	if (inputManager.GetKey(KeyCode::KEY_H).GetNumPressed() > 0)
	{
		m_gausBlur = std::max(m_gausBlur / 2, 1);
		// base radius is 3, is defined in shader
		printf("Gaus Radius is now is now: %i\n", m_gausBlur * 3);
	}

	if (inputManager.GetKey(KeyCode::PLUS).GetNumPressed() > 0)
	{
		m_rockBaseDensity = std::min(1.0f, m_rockBaseDensity + 0.05f);
		m_rock.GenerateMesh(m_mcLookup, m_rockBaseDensity);
		m_rock.GeneratedKdTree(rockModelMat);

		printf("Base Density is now: %f\n", m_rockBaseDensity);
	}
	if (inputManager.GetKey(KeyCode::MINUS).GetNumPressed() > 0)
	{
		m_rockBaseDensity = std::max(-1.0f, m_rockBaseDensity - 0.05f);
		m_rock.GenerateMesh(m_mcLookup, m_rockBaseDensity);
		m_rock.GeneratedKdTree(rockModelMat);

		printf("Base Density is now: %f\n", m_rockBaseDensity);
	}

	if (inputManager.GetKey(KeyCode::KEY_5).GetNumPressed() > 0)
	{
		m_dispScale = std::min(20.f, m_dispScale * 2);
		printf("Displacement hight scale: %f\n", m_dispScale);
	}
	if (inputManager.GetKey(KeyCode::KEY_6).GetNumPressed() > 0)
	{
		m_dispScale = std::max(0.f, m_dispScale / 2);
		printf("Displacement hight scale: %f\n", m_dispScale);
	}

	if (inputManager.GetKey(KeyCode::KEY_7).GetNumPressed() > 0)
	{
		m_dispRefinementLayers = std::min(20, m_dispRefinementLayers + 1);
		printf("Parallax refinement steps: %d\n", m_dispRefinementLayers);
	}
	if (inputManager.GetKey(KeyCode::KEY_8).GetNumPressed() > 0)
	{
		m_dispRefinementLayers = std::max(1, m_dispRefinementLayers - 1);
		printf("Parallax refinement steps: %d\n", m_dispRefinementLayers);
	}

	if (inputManager.GetKey(KeyCode::KEY_9).GetNumPressed() > 0)
	{
		m_dispLayers = std::min(20, m_dispLayers + 1);
		printf("Parallax main steps: %d\n", m_dispLayers);
	}

	if (inputManager.GetKey(KeyCode::KEY_0).GetNumPressed() > 0)
	{
		m_dispLayers = std::max(1, m_dispLayers - 1);
		printf("Parallax main steps: %d\n", m_dispLayers);
	}

	if (inputManager.GetKey(KeyCode::KEY_O).GetNumPressed() > 0)
	{
		m_numParticlesToSpawn = std::min(1, m_numParticlesToSpawn / 2);
		printf("Number Particles to Spawn %d\n", m_numParticlesToSpawn);
	}

	if (inputManager.GetKey(KeyCode::KEY_P).GetNumPressed() > 0)
	{
		m_numParticlesToSpawn*=2;
		printf("Number Particles to Spawn %d\n", m_numParticlesToSpawn);
	}

	if (inputManager.GetKey(KeyCode::KEY_U).GetNumPressed() > 0)
	{
		m_particleUpdateRateModifier *= 2.f;
		printf("Particle Update Rate modifier: %f\n", m_particleUpdateRateModifier);

	}

	if (inputManager.GetKey(KeyCode::KEY_I).GetNumPressed() > 0)
	{
		m_particleUpdateRateModifier *= 0.5f;
		printf("Particle Update Rate modifier: %f\n", m_particleUpdateRateModifier);

	}

	if (m_path.IsFollowingPath())
	{
		UpdatePathFollowing(deltaTime, inputManager);
	}
	else
	{
		m_pathFollower = PathFollower::none;
	}

	if (!m_path.IsFollowingPath() || m_pathFollower != PathFollower::camera)
	{
		UpdateFreeMovement(deltaTime, inputManager);
	}
}


void Scene::UpdateFreeMovement(float deltaTime, const InputManager & inputManager)
{
	if (inputManager.GetMouseMotion() != glm::vec2{ 0,0 } && !inputManager.GetKey(KeyCode::KEY_LEFT_SHIFT).IsPressed())
	{
		const glm::vec2 mouseMotion = inputManager.GetMouseMotion();

		const float scaling = 0.01f;
		m_camera.UpdateFromMouse(mouseMotion.x * scaling, mouseMotion.y * scaling);
	}

	float leftInput = 0.f;
	float forwardInput = 0.f;

	if (inputManager.GetKey(KeyCode::KEY_W).IsPressed())
	{
		forwardInput += deltaTime * 10.f;
	}
	if (inputManager.GetKey(KeyCode::KEY_S).IsPressed())
	{
		forwardInput -= deltaTime * 10.f;
	}

	if (inputManager.GetKey(KeyCode::KEY_A).IsPressed())
	{
		leftInput += deltaTime * 10.f;
	}
	if (inputManager.GetKey(KeyCode::KEY_D).IsPressed())
	{
		leftInput -= deltaTime * 10.f;
	}
	m_camera.UpdateLocation(forwardInput, leftInput);

	if (inputManager.GetKey(KeyCode::KEY_R).GetNumPressed() > 0)
	{
		RayTraceAndSpawnParticles(inputManager.GetMousePosition());
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

	/*if (inputManager.GetKey(KeyCode::PLUS).IsPressed())
	{
		m_currentRoughness = std::min(1.0f, m_currentRoughness + 0.05f);
	}
	if (inputManager.GetKey(KeyCode::MINUS).IsPressed())
	{
		m_currentRoughness = std::max(0.0f, m_currentRoughness - 0.05f);
	}*/

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


	if (m_useEsm)
	{
		EsmShadowMapPass();
	}
	else
	{
		for (int i = 0; i < LightCount; ++i)
		{
			if (m_isLightActive[i]) // save performance
			{
				ShadowMapPass(i);
			}
		}
	}

	RenderScenePass();
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
	m_shadowMapProgram.SetMatrixUniform(viewProjection, ViewProjectionUniformName);

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

void Scene::EsmShadowMapPass()
{
	glCullFace(GL_FRONT);

	m_esmShadowFrameBuffer.Bind();
	glViewport(0, 0, shadowDimensions.x, shadowDimensions.y);
	m_esmShadowMapProgram.UseProgram(); // TODO
	m_esmShadowMapProgram.SetFloatUniform(m_esmModifier, ESM_MODIFIER_UNIFORM_NAME);


	ASSERT_GL_ERROR_MACRO();

	m_esmShadowMapProgram.IsValid();
	for (int LightIndex = 0; LightIndex < LightCount; LightIndex++)
	{
		if (!m_isLightActive[LightIndex])
		{
			continue;
		}

		m_esmShadowFrameBuffer.BindTexture(GL_COLOR_ATTACHMENT0, m_esmShadowDepthTextures[LightIndex].GetHandle(), 0);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		m_esmShadowMapProgram.IsValid();
		glm::mat4 viewProjection = m_spotlight[LightIndex].GetPerspectionMatrix() * m_spotlight[LightIndex].CalcViewMatrix();
		m_esmShadowMapProgram.SetMatrixUniform(viewProjection, ViewProjectionUniformName);

		m_esmShadowMapProgram.SetMatrixUniform(glm::translate(glm::scale(glm::vec3{ 50.f,1.f,50.f }), glm::vec3(0.f, -10.f, 0.f)), MODEL_MATRIX_UNIFORM_NAME);
		m_cubeMesh.Render();

		for (const auto& cubeData : m_cubeShaderDatas)
		{
			m_esmShadowMapProgram.SetMatrixUniform(glm::translate(glm::mat4(1.0), cubeData.position), MODEL_MATRIX_UNIFORM_NAME);
			m_cubeMesh.Render();
		}
	}
	m_esmShadowFrameBuffer.Unbind();
	glCullFace(GL_BACK);


	for (int LightIndex = 0; LightIndex < LightCount; LightIndex++)
	{
		if (!m_isLightActive[LightIndex])
		{
			continue;
		}
		m_gausFilterer.Filter(m_esmShadowDepthTextures[LightIndex], m_esmShadowDepthTemporaryTexture, m_gausBlur);
	}
}


void Scene::RenderScenePass()
{
	if (m_useWireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}

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
	m_program.SetFloatUniform(m_esmModifier, ESM_MODIFIER_UNIFORM_NAME);

	glViewport(0, 0, m_viewPort.x, m_viewPort.y);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ASSERT_GL_ERROR_MACRO();

	int textureUnitCnt = 0;

	for (int i = 0; i < LightCount; ++i)
	{
		m_program.SetSamplerTextureUnit(textureUnitCnt, SHADOW_MAP_UNIFORM_NAME_ARRAY[i]);
		if (m_useEsm)
		{
			m_esmShadowDepthTextures[i].BindToTextureUnit(textureUnitCnt);
		}
		else
		{
			m_shadowDepthTexture[i].BindToTextureUnit(textureUnitCnt);
		}
		++textureUnitCnt;
	}


	glm::mat4 viewProjection = m_camera.GetPerspectionMatrix() * m_camera.CalcViewMatrix();

	m_program.SetMatrixUniform(viewProjection, ViewProjectionUniformName);

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



	m_rockShaderProgram.UseProgram();

	m_rockShaderProgram.SetIntUniform(m_dispLayers, DISPLACEMENT_LAYERS_UNIFORM_NAME);
	m_rockShaderProgram.SetIntUniform(m_dispRefinementLayers, DISPLACEMENT_REFINEMENT_LAYERS_UNIFORM_NAME);
	m_rockShaderProgram.SetFloatUniform(m_dispScale, DISPLACEMENT_HEIGHT_UNIFORM_NAME);


	m_rockShaderProgram.SetMatrixUniform(rockModelMat, MODEL_MATRIX_UNIFORM_NAME);

	const glm::mat4 invTranspRockModelMat = glm::transpose(glm::inverse(rockModelMat));
	m_rockShaderProgram.SetMatrixUniform(invTranspRockModelMat, INVERSE_TRANSPOSED_MODEL_MATRIX_UNIFORM_NAME);

	m_rockShaderProgram.SetSamplerTextureUnit(0, COLOR_TEX_UNIFORM_NAME);
	m_rockColor.BindToTextureUnit(0);

	m_rockShaderProgram.SetSamplerTextureUnit(1, NORMAL_TEX_UNIFORM_NAME);
	m_rockNormal.BindToTextureUnit(1);

	m_rockShaderProgram.SetSamplerTextureUnit(2, DISPLACEMENT_TEX_UNIFORM_NAME);
	m_rockDisp.BindToTextureUnit(2);
	
	m_rockShaderProgram.SetMatrixUniform(viewProjection, ViewProjectionUniformName);
	m_rockShaderProgram.SetVec3Uniform(m_camera.m_position, CAMERA_POS_UNIFORM_NAME);

	for (int i = 0; i < LightCount; ++i)
	{
		m_rockShaderProgram.SetVec3Uniform(m_spotlight[i].m_position, LIGHT_POS_UNIFORM_NAME_ARRAY[i]);
	}

	m_rock.Render();

	m_rockShaderProgram.UnuseProgram();

	m_terrain.Draw(viewProjection, m_spotlight.data(), m_spotlight.size());


	for (auto& ps : m_particleSystem)
	{
		ps.Draw(viewProjection);
	}


	DrawLines();

	if (m_allowedSampleSizes[m_sampleIndex] > 1)
	{
		FrameBufferUtils::CopyBufferData(m_msaaFrameBuffer, m_framebuffer, m_viewPort);
	}

	m_msaaFrameBuffer.Unbind();
	m_framebuffer.Unbind();

	if (m_useWireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

void Scene::PostProcessPass()
{
	glViewport(0, 0, m_viewPort.x, m_viewPort.y);

	m_postProcessProgram.UseProgram();

	m_postProcessProgram.SetSamplerTextureUnit(0, RENDERED_SCENE_TEXTURE_UNIFORM_NAME);
	m_colorTexture.BindToTextureUnit(0);

	for (int i = 0; i < LightCount; ++i)
	{
		m_postProcessProgram.SetSamplerTextureUnit(i + 1, SHADOW_MAP_UNIFORM_NAME_ARRAY[i]);
		m_shadowDepthTexture[i].BindToTextureUnit(i + 1);
	}

	m_postProcessProgram.IsValid();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	ASSERT_GL_ERROR_MACRO();

	m_screenTriangleMesh.Render();

}

void Scene::RayTraceAndSpawnParticles(glm::vec2 mousePos)
{
	std::printf("mousePos=%s\n", glm::to_string(mousePos).c_str());
	glm::vec2 nmp = normalizeMousePosition(mousePos, m_viewPort);
	nmp.y *= -1.f;

	const glm::vec3 direction = glm::normalize(screenToWorld(glm::vec3(nmp, 1.f), m_camera) - screenToWorld(glm::vec3(nmp, 0.f), m_camera));

	float length = 100;

	const auto result = KdTreeTraverser::FindHitTriangle(m_rock.m_kdTree, m_camera.m_position, direction, length);
	if (result.isValid())
	{
		m_particleSystem[m_currentParticleSystem].GenerateRandomParticels(result.intersectionPoint, m_numParticlesToSpawn);
		m_currentParticleSystem = (m_currentParticleSystem + 1) % m_particleSystem.size();
	}

	m_linesToDrawInWorldSpace.clear();
	const auto edgeLines = m_rock.m_kdTree.rootNode()->boundingBox.getEdgeLines();
	m_linesToDrawInWorldSpace.insert(m_linesToDrawInWorldSpace.begin(), edgeLines.begin(), edgeLines.end());
	m_linesToDrawInWorldSpace.push_back(m_camera.m_position);
	m_linesToDrawInWorldSpace.push_back(m_camera.m_position + length * direction);
}

void Scene::DrawLines()
{
	glLineWidth(2.f);
	LineMesh lines;
	lines.CreateInstanceOnGPU(m_linesToDrawInWorldSpace);
	//lines.CreateInstanceOnGPU({ LineData{glm::vec3(0.f,0.f,0.f), glm::vec3(0.f,1.f,0.f)} });
	m_lineShaderProgram.UseProgram();
	m_lineShaderProgram.SetVec4Uniform(glm::vec4(1, 0, 0, 1), COLOR_UNIFORM_NAME);
	glm::mat4 viewProjection = m_camera.GetPerspectionMatrix() * m_camera.CalcViewMatrix();
	m_lineShaderProgram.SetMatrixUniform(viewProjection, ViewProjectionUniformName);
	lines.Render();
}

