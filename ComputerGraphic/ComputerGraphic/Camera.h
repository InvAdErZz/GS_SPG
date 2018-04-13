#pragma once
#include "glm.hpp"
#include "Rotation.h"

class Camera
{
public:

	glm::mat4 CalcViewMatrix() const;

	void SetPerspection(float nearPlane, float farPlane, float fieldOfView, glm::vec2 aspectRatio);
	const glm::mat4& GetPerspectionMatrix() const;

	glm::quat m_rotation;
	glm::vec3 m_position;
private:

	glm::mat4 m_perspectionMatrix;
};

