#include "stdafx.h"
#include "Path.h"
#include "InputManager.h"
#include "Camera.h"
#include "SpotLight.h"

namespace
{
	constexpr int INVALID_PATH_INDEX = 0;


	float CalcDistanceBetweenKeyFrames(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3)
	{
		return glm::distance(v1, v2);
	}

	glm::quat Sqad(glm::quat q0, glm::quat q1, glm::quat q2, glm::quat q3, float fraction)
	{

		glm::quat intermediate1 = glm::intermediate(q0,q1,q2);
		glm::quat intermediate2 = glm::intermediate(q1,q2,q3);

		return glm::slerp(q1, q2, fraction);

		//return glm::squad(q1, q2, intermediate1, intermediate2, fraction);	
	}

	KeyFrame CalcInterpolation(const std::vector<KeyFrame>& keyframes, size_t pathIndex, float progressFraction)
	{
		assert(pathIndex > 0);
		assert(pathIndex + 2 < keyframes.size());

		assert(0.f <= progressFraction && progressFraction < 1.f);

		KeyFrame result;
		result.position = glm::catmullRom(
			keyframes[pathIndex - 1].position,
			keyframes[pathIndex].position,
			keyframes[pathIndex + 1].position,
			keyframes[pathIndex + 2].position,
			progressFraction
		);

		result.rotation = Sqad(keyframes[pathIndex - 1].rotation, keyframes[pathIndex].rotation, keyframes[pathIndex + 1].rotation, keyframes[pathIndex + 2].rotation, progressFraction);

		return result;
	}

	std::vector<CubeShaderData> GenPathCubes(const std::vector<KeyFrame>& keyframes)
	{
		const float interval = 2.f;
		float progress = 0;

		std::vector<CubeShaderData> pathCubes;

		for (size_t pathIndex = 1; pathIndex + 2 < keyframes.size(); ++pathIndex)
		{
			pathCubes.emplace_back(keyframes[pathIndex].position, glm::vec4(0.0f, 0.f, 1.f, 1.f), 0);

			float splineLength = CalcDistanceBetweenKeyFrames(
				keyframes[pathIndex - 1].position,
				keyframes[pathIndex].position,
				keyframes[pathIndex + 1].position,
				keyframes[pathIndex + 2].position
			);

			progress += interval;
			while (progress < splineLength)
			{

				KeyFrame interpolation = CalcInterpolation(keyframes, pathIndex, progress / splineLength);
				pathCubes.emplace_back(interpolation.position, glm::vec4(0.0f, 1.f, 0.f, 1.f), 0);
				progress += interval;
			}

			progress -= splineLength;
		}
		pathCubes.emplace_back(keyframes[keyframes.size() - 2].position, glm::vec4(0.0f, 0.f, 1.f, 1.f), 0);
		return pathCubes;
	}

	size_t UpdatePath_Internal(const float deltaTime,
		const float pathSpeed, size_t pathIndex,
		const std::vector<KeyFrame>& keyframes,
		float& interpolationProgress,
		float& currentInterpolationLength)
	{
		interpolationProgress += pathSpeed * deltaTime;
		while (interpolationProgress >= currentInterpolationLength)
		{
			if (pathIndex + 3 >= keyframes.size())
			{
				return INVALID_PATH_INDEX;
			}

			interpolationProgress -= currentInterpolationLength;
			++pathIndex;

			currentInterpolationLength = CalcDistanceBetweenKeyFrames(
				keyframes[pathIndex - 1].position,
				keyframes[pathIndex].position,
				keyframes[pathIndex + 1].position,
				keyframes[pathIndex + 2].position
			);
		}
		return pathIndex;
	}

}


bool Path::IsFollowingPath() const
{
	return m_pathIndex != INVALID_PATH_INDEX;
}

bool Path::TryStartPathFollowing()
{
	assert(!IsFollowingPath());
	if (m_keyFrames.size() < 3)
	{
		return false;
	}
	KeyFrame last = m_keyFrames.back();
	m_keyFrames.push_back(last);
	m_pathcubes = GenPathCubes(m_keyFrames);

	m_pathIndex = 1;
	return true;
}

void Path::StopPathFollowing()
{
	assert(IsFollowingPath());
	m_keyFrames.pop_back();
	assert(m_keyFrames.size() >= 3);
	m_pathcubes.clear();

	m_pathIndex = INVALID_PATH_INDEX;
}

void Path::UpdatePathFollowing(float deltaTime, const InputManager& inputManager, Camera& cameraToUpdate)
{
	if (inputManager.GetKey(KeyCode::KEY_W).IsPressed())
	{
		m_pathSpeed += deltaTime * 1.f;
	}
	if (inputManager.GetKey(KeyCode::KEY_S).IsPressed())
	{
		m_pathSpeed -= deltaTime * 1.f;
	}

	m_pathSpeed = glm::clamp(m_pathSpeed, 0.f, 20.f);

	size_t newPathIndex = UpdatePath_Internal(deltaTime, m_pathSpeed, m_pathIndex, m_keyFrames, m_currentInterpolationProgress, m_currentInterpolationLength);
	if (newPathIndex == INVALID_PATH_INDEX)
	{
		StopPathFollowing();
		return;
	}
	m_pathIndex = newPathIndex;

	const float progressFraction = m_currentInterpolationProgress / m_currentInterpolationLength;;
	KeyFrame interpolationPoint = CalcInterpolation(m_keyFrames,m_pathIndex, progressFraction);
	cameraToUpdate.m_position = interpolationPoint.position;
	cameraToUpdate.m_rotation = interpolationPoint.rotation;
}

void Path::UpdatePathFollowing(float deltaTime, SpotLight& spotlightToUpdate)
{
	m_pathIndex = UpdatePath_Internal(deltaTime, m_pathSpeed, m_pathIndex, m_keyFrames, m_currentInterpolationProgress, m_currentInterpolationLength);
	size_t newPathIndex = UpdatePath_Internal(deltaTime, m_pathSpeed, m_pathIndex, m_keyFrames, m_currentInterpolationProgress, m_currentInterpolationLength);
	if (newPathIndex == INVALID_PATH_INDEX)
	{
		StopPathFollowing();
		return;
	}
	m_pathIndex = newPathIndex;

	const float progressFraction = m_currentInterpolationProgress / m_currentInterpolationLength;;
	KeyFrame interpolationPoint = CalcInterpolation(m_keyFrames, m_pathIndex, progressFraction);
	spotlightToUpdate.m_position = interpolationPoint.position;
	spotlightToUpdate.m_direction = interpolationPoint.rotation * glm::vec3(0.f, 0.f, 1.f);
}

void Path::AddKeyFrame(const glm::quat& rotation, const glm::vec3& positon)
{
	KeyFrame keyFrame = { rotation, positon };
	m_keyFrames.push_back(keyFrame);
	if (m_keyFrames.size() == 1)
	{
		m_keyFrames.push_back(keyFrame);
	}
}

KeyFrame Path::RemoveLastKeyFrame()
{
	KeyFrame result;
	assert(m_keyFrames.size() > 0);

	result = m_keyFrames.back();
	m_keyFrames.pop_back();
	if (m_keyFrames.size() == 1)
	{
		m_keyFrames.pop_back();
		assert(m_keyFrames.size() == 0);
	}

	return result;
}


