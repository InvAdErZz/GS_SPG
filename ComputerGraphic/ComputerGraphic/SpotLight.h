#pragma once
#include "glm.hpp"

class SpotLight
{
public:

	glm::mat4 CalcViewMatrix() const;

	void SetPerspection(float nearPlane, float farPlane, float fieldOfView_degrees);
	const glm::mat4& GetPerspectionMatrix() const;

	glm::vec3 m_direction;
	glm::vec3 m_position;
private:

	float m_fieldOfView_radiens;
	glm::mat4 m_perspectionMatrix;
};
