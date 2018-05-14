#pragma once
#include "glm.hpp"
#include <array>

struct Triangle;

struct AABB {
	glm::vec3 bbMin;
	glm::vec3 bbMax;

	void expand(glm::vec3 v);
	bool containsPoint(glm::vec3 v) const;
	bool intersectsTriangle(const Triangle& tri) const;
	int dimToSplit() const;
	std::array<glm::vec3, 24> getEdgeLines() const;
};
