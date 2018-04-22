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
	void RenderScenePass();
	void PostProcessPass();

	Mesh m_cubeMesh;
	MeshIndexed m_screenTriangleMesh;
	ProceduralMesh m_rock;

	std::vector<CubeShaderData> m_cubeShaderDatas;

	Camera m_camera;
	ShaderProgram m_program;
	ShaderProgram m_shadowMapProgram;
	ShaderProgram m_postProcessProgram;
	ShaderProgram m_rockShaderProgram;

	FrameBuffer m_framebuffer;
	FrameBuffer m_msaaFrameBuffer;
	LookupBuffer m_mcLookup;

	Texture2DMultisample m_msaaColorTexture;
	Texture2DMultisample m_msaaDepthTexture;

	Texture m_colorTexture;
	Texture m_depthTexture;
	std::array<Texture, 3> m_normalMaps;

	std::array<FrameBuffer,LightCount> m_shadowFrameBuffer;
	std::array<Texture, LightCount> m_shadowDepthTexture;
	std::array<SpotLight, LightCount> m_spotlight;
	std::array<bool, LightCount> m_isLightActive;

	Path m_path;
	PathFollower m_pathFollower = PathFollower::none;
	
	glm::ivec2 m_viewPort;
	float m_currentRoughness = 0.5f;
	bool m_msaaFixedSamples = false;
	int m_sampleIndex = 0;
	std::vector<GLsizei> m_allowedSampleSizes;
};
