#include "stdafx.h"
#include "SpotLight.h"

glm::mat4 SpotLight::CalcViewMatrix() const
{
	return glm::lookAt(m_position, m_position + m_direction, glm::vec3(0.f, 1.f, 0.f));
}

void SpotLight::SetPerspection(float nearPlane, float farPlane, float fieldOfView_degrees)
{
	m_fieldOfView_radiens = glm::radians(fieldOfView_degrees);
	m_perspectionMatrix = glm::perspective(m_fieldOfView_radiens, 1.f, nearPlane, farPlane);
}

const glm::mat4 & SpotLight::GetPerspectionMatrix() const
{
	return m_perspectionMatrix;
}
