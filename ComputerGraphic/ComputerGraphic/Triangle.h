#pragma once
#include <array>
#include "glm.hpp"

struct Triangle
{
	std::array<glm::vec3, 3> data;
	Triangle() = default;
	Triangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3) : data({ v1,v2,v3 }) {}

	const glm::vec3& operator[](int index) const { return data[index]; }

	glm::vec3 v0() const { return data[0]; }
	glm::vec3 v1() const { return data[1]; }
	glm::vec3 v2() const { return data[2]; }

	glm::vec3 centroid() const {
		return (v0() + v1() + v2()) / 3.0f;
	}
};