#include "stdafx.h"
#include "KdTree.h"
#include <algorithm>
#include <numeric>

namespace
{

	AABB makeBoundingBox(const std::vector<Triangle>& triangles) {
		AABB ab;
		ab.bbMin = triangles[0].data[0];
		ab.bbMax = ab.bbMin;

		for (auto tri : triangles) {
			for (auto vec : tri.data) {
				ab.expand(vec);
			}
		}

		return ab;
	}

	float findSplitValue(const std::vector<int>& indices, const std::vector<Triangle>& triangles, int dim)
	{
		const int triCount = indices.size();
		std::vector<glm::vec3> centroids;
		centroids.reserve(triCount);
		for (int idx : indices)
		{
			centroids.push_back(triangles[idx].centroid());
		}

		std::nth_element(centroids.begin(), centroids.begin() + (triCount / 2), centroids.end(), [dim](const glm::vec3& c1, const glm::vec3& c2) {
			
			return c1[dim] < c2[dim];
		});
		
		return centroids[triCount / 2][dim];
	}

	std::pair<std::vector<int>, std::vector<int>> splitTriangles(const std::vector<int>& indices, const std::vector<Triangle>& triangles, int dim, float splitvalue)
	{
		std::pair<std::vector<int>, std::vector<int>> result;
		result.first.reserve(indices.size());
		result.second.reserve(indices.size());

		for (int tIdx : indices) {
			const Triangle& tri = triangles[tIdx];

			if (tri[0][dim] < splitvalue
				|| tri[1][dim] < splitvalue
				|| tri[2][dim] < splitvalue)
			{
				result.first.push_back(tIdx);
			}

			if (tri[0][dim] > splitvalue
				|| tri[1][dim] > splitvalue
				|| tri[2][dim] > splitvalue)
			{
				result.second.push_back(tIdx);
			}
		}
		return result;
	}

	std::unique_ptr<KdNode> createLeafNode(const std::vector<int>& indices, const AABB& boundingBox)
	{
		std::unique_ptr<KdNode> resultNode = std::make_unique<KdNode>();
		resultNode->axisSplit = -1;
		resultNode->boundingBox = boundingBox;
		resultNode->triangleIndices = indices;
		return resultNode;
	}

	std::unique_ptr<KdNode> createTreeRecursive(std::vector<int> indices, const std::vector<Triangle>& triangles, const AABB& boundingBox )
	{
		int triCount = indices.size();
		if (triCount < 10)
		{
			return createLeafNode(indices, boundingBox);
		}

		int dimToSplit = boundingBox.dimToSplit();
		
		
		float valToSplit = findSplitValue(indices, triangles, dimToSplit);
		auto splitTries = splitTriangles(indices, triangles, dimToSplit, valToSplit);

		// split was not successful
		if (splitTries.first.size() == triCount || splitTries.second.size() == triCount)
		{
			return createLeafNode(indices, boundingBox);
		}

		AABB bb1 = boundingBox;
		AABB bb2 = boundingBox;
		bb1.bbMax[dimToSplit] = valToSplit;
		bb2.bbMin[dimToSplit] = valToSplit;

		std::unique_ptr<KdNode> resultNode = std::make_unique<KdNode>();
		resultNode->axisSplit = dimToSplit;
		resultNode->splitValue = valToSplit;
		resultNode->child[0] = createTreeRecursive(std::move(splitTries.first), triangles, bb1);
		resultNode->child[1] = createTreeRecursive(std::move(splitTries.second), triangles, bb2);
		resultNode->boundingBox = boundingBox;

		return resultNode;
	}
}


void KdTree::resetTree()
{
	m_triangles.clear();
	m_node.reset();
	m_nameMapping.clear();

}

void KdTree::addCollisionMesh(const std::vector<Triangle>& triangles, std::string name)
{
	if (triangles.size() == 0) {
		return;
	}

	int startIndex = m_triangles.size();
	assert(rangeOfName(name).length == 0);
	m_triangles.insert(m_triangles.end(), triangles.begin(), triangles.end());
	
	m_nameMapping.emplace_back(name, range(startIndex, triangles.size()));
}

void KdTree::generateTree()
{
	AABB InitialBB = makeBoundingBox(m_triangles);
	std::vector<int> intialIndices;
	intialIndices.resize(m_triangles.size());
	std::iota(intialIndices.begin(), intialIndices.end(), 0);

	m_node = createTreeRecursive(std::move(intialIndices), m_triangles, InitialBB);
}

std::pair<std::string, int> KdTree::getOwnerAndIndex(int triangleIndex) const
{
	for (auto it = m_nameMapping.crbegin(); it != m_nameMapping.crend(); ++it)
	{
		if (it->second.start < triangleIndex)
		{
			assert(triangleIndex < it->second.start + it->second.length);
			return std::make_pair(it->first, triangleIndex - it->second.start);
		}
	}

	return std::pair<std::string, int>();
}

void KdTree::generateNode(KdNode* node)
{
	auto& tIndices = node->triangleIndices;
	size_t count = tIndices.size();
	if (count < 10) {
		return;
	}
	int dimToSplit = node->boundingBox.dimToSplit();
	std::nth_element(tIndices.begin(), tIndices.begin() + (count / 2), tIndices.end(), [this, dimToSplit](int t1Idx, int t2Idx) {
		auto& t1 = m_triangles[t1Idx];
		auto& t2 = m_triangles[t2Idx];
		glm::vec3 c1 = t1.centroid();
		glm::vec3 c2 = t2.centroid();
		return c1[dimToSplit] < c2[dimToSplit];
	});

	Triangle& midTri = m_triangles[tIndices[count / 2]];
	float valToSplit = midTri.centroid()[dimToSplit];
	splitNode(node, dimToSplit, valToSplit);

	generateNode(node->child[0].get());
	generateNode(node->child[1].get());
}

void KdTree::splitNode(KdNode* node, int axisSplit, float valueToSplit)
{
	node->child[0] = std::make_unique<KdNode>();
	node->child[1] = std::make_unique<KdNode>();
	node->splitValue = valueToSplit;

	AABB bb1 = node->boundingBox;
	AABB bb2 = node->boundingBox;
	bb1.bbMax[axisSplit] = valueToSplit;
	bb2.bbMin[axisSplit] = valueToSplit;

	node->child[0]->boundingBox = bb1;
	node->child[1]->boundingBox = bb2;

	for (int tIdx : node->triangleIndices) {
		auto t = m_triangles[tIdx];
		if (node->child[0]->boundingBox.intersectsTriangle(t)) {
			node->child[0]->triangleIndices.push_back(tIdx);
		}
		if (node->child[1]->boundingBox.intersectsTriangle(t)) {
			node->child[1]->triangleIndices.push_back(tIdx);
		}
	}

	//node->triangleIndices.clear();
}

KdTree::range KdTree::rangeOfName(std::string name) const
{
	for (int i = 0; i < m_nameMapping.size(); i++) {
		if (m_nameMapping[i].first == name) {
			return m_nameMapping[i].second;
		}
	}
	return range();
}
