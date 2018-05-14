#pragma once
#include "glm.hpp"

#include "Triangle.h"
#include <limits>

struct RayTriangleIntersector
{
	RayTriangleIntersector(const glm::vec3& rayStart, const glm::vec3& rayDirection)
		: rayStart(rayStart)
		, rayDirection(rayDirection){}

	glm::vec3 rayStart;
	glm::vec3 rayDirection;
	
	// successful when intersection happened
	bool isIntersecting(const Triangle& triangle, glm::vec3& intersectionPoint) const
	{
		// Möller–Trumbore intersection algorithm
		// https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm

		const float epsilon = 0.0000001f;

		const glm::vec3 edge1 = triangle[1] - triangle[0];
		const glm::vec3 edge2 = triangle[2] - triangle[0];

		const glm::vec3 h = glm::cross(rayDirection, edge2);
		float a = glm::dot(edge1, h);


		if (a > -epsilon && a < epsilon)
			return false;

		const float f = 1 / a;
		const glm::vec3 s = rayStart - triangle[0];

		const float u = f * glm::dot(s, h);
		if (u < 0.0 || u > 1.0)
			return false;

		const glm::vec3 q = glm::cross(s, edge1);
		const float v = f * glm::dot(rayDirection, q);
		if (v < 0.0 || u + v > 1.0)
			return false;

		// At this stage we can compute t to find out where the intersection point is on the line.
		const float t = f * glm::dot(edge2, q);
		if (t > epsilon) // ray intersection
		{
			intersectionPoint = rayStart + rayDirection * t;
			return true;
		}
		else // This means that there is a line intersection but not a ray intersection.
			return false;
	}

	// successful when intersection happened
	bool isIntersecting(const Triangle& triangle) const
	{
		// Möller–Trumbore intersection algorithm
		// https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm

		glm::vec3 intersectionPoint;
		bool ii = isIntersecting(triangle, intersectionPoint);
		return ii;
	}
};
