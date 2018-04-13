#pragma once
#include <vector>
#include "CubeShaderData.h"
#include "glm.hpp"
#include "ext.hpp"

class InputManager;
class Camera;
class SpotLight;

struct KeyFrame
{
	glm::quat rotation;
	glm::vec3 position;
};


class Path
{
public:
	bool IsFollowingPath() const;
	bool TryStartPathFollowing();
	void StopPathFollowing();
	void UpdatePathFollowing(float deltaTime, const InputManager& inputManager, Camera& cameraToUpdate);
	void UpdatePathFollowing(float deltaTime, SpotLight& spotlightToUpdate);

	void AddKeyFrame(const glm::quat& rotation, const glm::vec3& positon);
	KeyFrame RemoveLastKeyFrame();
	bool HasKeyFrames() const { return m_keyFrames.size() > 0; }

	const std::vector<CubeShaderData>& GetPathCubes() const { return m_pathcubes; }
private:
	std::vector<CubeShaderData> m_pathcubes;
	std::vector<KeyFrame> m_keyFrames;

	size_t m_pathIndex;
	float m_pathSpeed = 5.f;
	float m_currentInterpolationProgress;
	float m_currentInterpolationLength;
};

