#include "stdafx.h"
#include "Camera.h"

glm::mat4 Camera::CalcViewMatrix() const
{
	glm::mat4 rotation = glm::inverse(glm::mat4_cast(m_rotation));
	glm::mat4 translation = glm::translate(-m_position);

	auto rotationMat = glm::mat3_cast(m_rotation);
	glm::vec3 forwardVector = rotationMat * glm::vec3(0, 0, 1);
	glm::vec3 upVector = rotationMat * glm::vec3(0, 1, 0);

	return glm::lookAt(m_position, m_position + forwardVector, upVector);
}

void Camera::SetPerspection(float nearPlane, float farPlane, float fieldOfView, glm::vec2 aspectRatio)
{
	m_perspectionMatrix = glm::perspectiveFov(fieldOfView, aspectRatio.x, aspectRatio.y, nearPlane, farPlane);
}

const glm::mat4 & Camera::GetPerspectionMatrix() const
{
	return m_perspectionMatrix;
}
