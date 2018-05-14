#pragma once

#include "KdTree.h"
#include "glm.hpp"

#include <vector>
#include <string>
#include "RayTriangleIntersector.h"

namespace KdTreeTraverser
{
	void VisitNodes(const KdNode* pNode, const glm::vec3 point, const glm::vec3 direction, const float tmax, std::vector<int>& outIndices);

	struct triangleHitResult {
		std::string name = "";
		int index = -1;
		glm::vec3 intersectionPoint = glm::vec3(0,0,0);
		triangleHitResult() = default;
		triangleHitResult(const std::string& name_, int index_, glm::vec3 intersectionPoint_) : name(name_), index(index_), intersectionPoint(intersectionPoint_) {}

		bool isValid() const { return (index >= 0); }
	};
	
	triangleHitResult FindHitTriangle(const KdTree& tree, glm::vec3 raycastStart, glm::vec3 raycastDirection, float raycastLength);
	std::vector<glm::vec3> GenerateWireFrame(const KdTree& tree);
}
