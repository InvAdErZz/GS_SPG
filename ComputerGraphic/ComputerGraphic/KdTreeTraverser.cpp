#include "stdafx.h"
#include "KdTreeTraverser.h"

namespace
{
	const int numCubeEdges = 24;

	void AppendNodeRecursive(const KdNode& node, std::vector<glm::vec3>& out)
	{
		out.reserve(numCubeEdges);
		auto edgeLines = node.boundingBox.getEdgeLines();
		out.insert(out.end(), edgeLines.begin(), edgeLines.end());

		if (node.axisSplit != -1) // no axis split
		{
			assert(node.child[0] != nullptr);
			assert(node.child[1] != nullptr);

			AppendNodeRecursive(*node.child[0], out);
			AppendNodeRecursive(*node.child[1], out);
		}		
	}
}


void KdTreeTraverser::VisitNodes(const KdNode * pNode, const glm::vec3 point, const glm::vec3 direction, const float tmax, std::vector<int>& outIndices)
{
	if (pNode == nullptr) return;
	// Visit current node ...
	// which child to recurse first into? (0=near,1=far		)
	const int dim = pNode->axisSplit;

	if (-1 == dim)
	{
		outIndices.insert(outIndices.end(), pNode->triangleIndices.begin(), pNode->triangleIndices.end());
		return;
	}


	const int first = point[dim] > pNode->splitValue;
	const int second = first ^ 1;

	if (direction[dim] == 0.0f) {
		// line segment parallel to splitting plane, visit 	near side only
		VisitNodes(pNode->child[first].get(), point, direction, tmax, outIndices);
	}
	else {
		//find t value for intersection
		const float t = (pNode->splitValue - point[dim]) / direction[dim];

		if (0.0f <= t && t < tmax) {
			VisitNodes(pNode->child[first].get(), point, direction, t, outIndices);
			VisitNodes(pNode->child[second].get(), point + t * direction, direction, tmax - t, outIndices);
		}
		else {
			VisitNodes(pNode->child[first].get(), point, direction, tmax, outIndices);
		}
	}
}

KdTreeTraverser::triangleHitResult KdTreeTraverser::FindHitTriangle(const KdTree & tree, glm::vec3 raycastStart, glm::vec3 raycastDirection, float raycastLength)
{
	//
	// Local variables
	//

	// Intersection result
	struct IR {
		// A triangle index
		int idx;
		// The intersection point
		glm::vec3 ip;
		IR() = default;
		IR(int idx, glm::vec3 ip) : idx(idx), ip(ip) {}
	};
	// Indices of triangles
	std::vector<int> triIndices;
	/// intersection result list
	std::vector<IR> irl;

	//
	// Filter triangles using kd-tree (writes to 'triIndices')
	//

	VisitNodes(tree.rootNode(), raycastStart, raycastDirection, raycastLength, triIndices);
	std::sort(triIndices.begin(), triIndices.end());

	//
	// Remove duplicate indices (from 'triIndices')
	//

	const auto endIt = std::unique(triIndices.begin(), triIndices.end());
	triIndices.erase(endIt, triIndices.end());

	//
	// Check all remaining triangles (writes to 'irl')
	//

	const RayTriangleIntersector raycastIntersector(raycastStart, raycastDirection);

	for (auto ti : triIndices) {
		glm::vec3 intersectionPoint;
		if (raycastIntersector.isIntersecting(tree.getTriangele(ti), intersectionPoint)) {
			irl.emplace_back(ti, intersectionPoint);
		}
	}

	//
	// Sort triangles by distance from 'raycastStart' point and return the nearest one.
	// (Sort 'irl' by distance to 'raycastStart')
	//

	if (irl.size() > 0) {
		auto ir = std::min_element(irl.begin(), irl.end(), [raycastStart](IR& a, IR& b) {
			float da = glm::distance2(a.ip, raycastStart);
			float db = glm::distance2(b.ip, raycastStart);
			return da < db;
		});
		auto r = tree.getOwnerAndIndex(ir->idx);
		return triangleHitResult(r.first, r.second, ir->ip);
	}
	else {
		return triangleHitResult();
	}
}

std::vector<glm::vec3> KdTreeTraverser::GenerateWireFrame(const KdTree& tree)
{
	std::vector<glm::vec3> result;	
	if (const KdNode* currentNode = tree.rootNode())
	{
		AppendNodeRecursive(*currentNode, result);
	}
	return result;
}
