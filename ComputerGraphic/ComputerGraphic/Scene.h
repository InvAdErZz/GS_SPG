#pragma once
#include "glm.hpp"

#include "MeshIndexed.h"
#include "Mesh.h"
#include "Camera.h"
#include "ShaderProgram.h"
#include <vector>
#include "FrameBuffer.h"
#include "Texture.h"
#include "CubeShaderData.h"
#include "Path.h"
#include "SpotLight.h"
#include <array>
#include "RenderBuffer.h"
#include "Texture2DMutlisample.h"
#include "Texture3d.h"
#include "LookupBuffer.h"
#include "ProceduralMesh.h"
#include "ParticleSystem.h"
#include "GausFilterer.h"
#include "Terrain.h"

enum class PathFollower
{
	none,
	camera,
	light0,
	light1
};

class InputManager;

class Scene
{
	
public:
	constexpr static int LightCount = 2;
	void Init(const glm::ivec2& ViewPort);
	void Update(float deltaTime, const InputManager& inputManager);
	void Render();
	void SetMsaaSamplingNumber(GLsizei samples, bool fixedSampleLocations);
private:
	void UpdateFreeMovement(float deltaTime, const InputManager& inputManager);
	void UpdatePathFollowing(float deltaTime, const InputManager& inputManager);

	void ShadowMapPass(int LightIndex);
	void EsmShadowMapPass();
	void RenderScenePass();
	void PostProcessPass();
	void RayTraceAndSpawnParticles(glm::vec2 mousePos);
	void DrawLines();

	GausFilterer m_gausFilterer;

	Mesh m_cubeMesh;
	MeshIndexed m_screenTriangleMesh;
	ProceduralMesh m_rock;

	std::vector<CubeShaderData> m_cubeShaderDatas;

	Camera m_camera;
	ShaderProgram m_program;
	ShaderProgram m_shadowMapProgram;
	ShaderProgram m_esmShadowMapProgram;
	ShaderProgram m_postProcessProgram;
	ShaderProgram m_rockShaderProgram;

	ShaderProgram m_lineShaderProgram;

	Terrain m_terrain;

	FrameBuffer m_framebuffer;
	FrameBuffer m_esmShadowFrameBuffer;
	FrameBuffer m_msaaFrameBuffer;
	LookupBuffer m_mcLookup;

	Texture2DMultisample m_msaaColorTexture;
	Texture2DMultisample m_msaaDepthTexture;

	Texture m_colorTexture;
	Texture m_depthTexture;
	Texture m_rockColor;
	Texture m_rockNormal;
	Texture m_rockDisp;
	Texture m_esmDepthTexture;

	std::array<Texture, 3> m_normalMaps;

	std::array<FrameBuffer,LightCount> m_shadowFrameBuffer;
	std::array<Texture, LightCount> m_shadowDepthTexture;
	std::array<Texture, LightCount> m_esmShadowDepthTextures;
	// used for bluring
	Texture m_esmShadowDepthTemporaryTexture;
	std::array<SpotLight, LightCount> m_spotlight;
	std::array<bool, LightCount> m_isLightActive;

	Path m_path;
	PathFollower m_pathFollower = PathFollower::none;

	std::array<ParticleSystem,3> m_particleSystem;
	int m_currentParticleSystem = 0;
	
	glm::ivec2 m_viewPort;
	float m_currentRoughness = 0.5f;
	bool m_msaaFixedSamples = false;
	int m_sampleIndex = 0;
	float m_rockBaseDensity = 0.f;
	float m_particleUpdateRateModifier = 1.f;
	bool m_useWireframe = false;

	int m_numParticlesToSpawn = 20;
	int m_dispLayers = 2;
	int m_dispRefinementLayers = 2;
	float m_dispScale = 0.0625;
	bool m_useEsm;
	float m_esmModifier;
	int m_gausBlur;
	bool m_printFramerate = false;
	std::vector<GLsizei> m_allowedSampleSizes;

	std::vector<glm::vec3> m_linesToDrawInWorldSpace;
};
