#pragma once
#include "Triangle.h"
#include <vector>

#include <memory>
#include <algorithm>
#include <string>
#include <array>
#include "AABB.h"


struct KdNode {
	AABB boundingBox;
	int axisSplit = -1;
	float splitValue;
	std::vector<int> triangleIndices;

	std::array<std::unique_ptr<KdNode>, 2> child;
};

class KdTree
{
public:
	struct range {
		int start = 0;
		int length = 0;
		range() = default;
		range(int s, int l) : start(s), length(l) {};
	};
	void resetTree();
	void addCollisionMesh(const std::vector<Triangle>& triangles, std::string name);
	void generateTree();
	const KdNode* rootNode() const { return m_node.get(); }
	const Triangle& getTriangele(int index) const { return m_triangles[index]; }
	std::pair<std::string, int> getOwnerAndIndex(int triangleIndex) const;

private:
	void generateNode(KdNode* node);

	void splitNode(KdNode* node, int axisSplit, float valueToSplit);

	std::vector<Triangle> m_triangles;
	std::unique_ptr<KdNode> m_node;

	std::vector<std::pair<std::string, range>> m_nameMapping;

	range rangeOfName(std::string name) const;
};
