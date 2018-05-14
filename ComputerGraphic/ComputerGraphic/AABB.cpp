#include "stdafx.h"
#include "AABB.h"
#include "Triangle.h"

void AABB::expand(glm::vec3 v) {
	for (int dimIdx = 0; dimIdx < 3; dimIdx++) {
		auto vi = v[dimIdx];
		if (vi < bbMin[dimIdx]) {
			bbMin[dimIdx] = vi;
		}
		if (vi > bbMax[dimIdx]) {
			bbMax[dimIdx] = vi;
		}
	}
}

bool AABB::containsPoint(glm::vec3 v) const {
	for (int dimIdx = 0; dimIdx < 3; dimIdx++) {
		auto vi = v[dimIdx];
		if (vi < bbMin[dimIdx]) {
			return false;
		}
		if (vi > bbMax[dimIdx]) {
			return false;
		}
	}
	return true;
}

bool AABB::intersectsTriangle(const Triangle & tri) const {
	return containsPoint(tri.data[0]) || containsPoint(tri.data[1]) || containsPoint(tri.data[2]);
}

int AABB::dimToSplit() const {
	glm::vec3 v = bbMax - bbMin;

	int idx = 0;
	float val = v.x;

	if (val < v.y) {
		val = v.y;
		idx = 1;
	}
	if (val < v.z) {
		val = v.z;
		idx = 2;
	}

	return idx;
}

std::array<glm::vec3, 24> AABB::getEdgeLines() const
{
	std::array<glm::vec3, 24> result;
	glm::vec3 v;
	int resIdx = 0;
	for (int dimA = 0; dimA < 3; dimA++) {
		int dimB = (dimA + 1) % 3;
		int dimC = (dimA + 2) % 3;
		
		// Line 1
		v = bbMin;
		result[resIdx++] = v;
		v[dimA] = bbMax[dimA];
		result[resIdx++] = v;

		// Line 2
		v[dimB] = bbMax[dimB];
		result[resIdx++] = v;
		v[dimA] = bbMin[dimA];
		result[resIdx++] = v;

		// Line 3
		v[dimC] = bbMax[dimC];
		result[resIdx++] = v;
		v[dimA] = bbMax[dimA];
		result[resIdx++] = v;

		// Line 4
		v[dimB] = bbMin[dimB];
		result[resIdx++] = v;
		v[dimA] = bbMin[dimA];
		result[resIdx++] = v;
	}

	assert(resIdx == 24);

	return result;
}

