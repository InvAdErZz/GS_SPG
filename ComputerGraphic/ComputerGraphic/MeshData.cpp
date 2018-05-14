#include "stdafx.h"
#include "MeshData.h"

#include "tiny_obj_loader.h"
#include <iostream>


namespace
{

	void CalcTangents(std::vector<std::vector<VertextAttribute>>& attributesCollection)
	{

		for (auto& attributs : attributesCollection)
		{
			assert(attributs.size() % 3 == 0);
			for (size_t i = 0; i < attributs.size(); i += 3)
			{
				// Shortcuts for vertices
				glm::vec3 & v0 = attributs[i + 0].position;
				glm::vec3 & v1 = attributs[i + 1].position;
				glm::vec3 & v2 = attributs[i + 2].position;

				// Shortcuts for UVs
				glm::vec2 & uv0 = attributs[i + 0].texcoord;
				glm::vec2 & uv1 = attributs[i + 1].texcoord;
				glm::vec2 & uv2 = attributs[i + 2].texcoord;

				// Edges of the triangle : postion delta
				glm::vec3 deltaPos1 = v1 - v0;
				glm::vec3 deltaPos2 = v2 - v0;

				// UV delta
				glm::vec2 deltaUV1 = uv1 - uv0;
				glm::vec2 deltaUV2 = uv2 - uv0;

				glm::vec3 tangent = glm::normalize(deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y);
				glm::vec3 bitangent = glm::normalize(deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x);			

				if (glm::dot(glm::cross(attributs[i + 0].normal, tangent), bitangent) < 0.0f) 
				{
					tangent *= -1.f;
				}
			
#if 0
				assert(std::abs(glm::dot(tangent, bitangent)) < 0.001f);
				assert(std::abs(glm::dot(attributs[i].normal, tangent)) < 0.001f);
#endif
				attributs[i + 0].tangent = tangent;
				attributs[i + 1].tangent = tangent;
				attributs[i + 2].tangent = tangent;

				attributs[i + 0].bitangent = bitangent;
				attributs[i + 1].bitangent = bitangent;
				attributs[i + 2].bitangent = bitangent;
			}

		}

	}

}














MeshData MeshCreation::CreateRectangle_indexed(glm::vec3 halfLength)
{
	MeshData result;
	//positon / color
	{ 
		auto& vertexData = result.vertexData;
		vertexData.resize(8);
		vertexData[0].position = { -halfLength.x, -halfLength.y, -halfLength.z };
		vertexData[1].position = { -halfLength.x, -halfLength.y, +halfLength.z };
		vertexData[2].position = { -halfLength.x, +halfLength.y, -halfLength.z };
		vertexData[3].position = { -halfLength.x, +halfLength.y, +halfLength.z };
		vertexData[4].position = { +halfLength.x, -halfLength.y, -halfLength.z };
		vertexData[5].position = { +halfLength.x, -halfLength.y, +halfLength.z };
		vertexData[6].position = { +halfLength.x, +halfLength.y, -halfLength.z };
		vertexData[7].position = { +halfLength.x, +halfLength.y, +halfLength.z };

		/*for (auto& element : vertexData)
		{
			element.color = { 1.f,0.f,0.f,1.f };
		}*/
	}
	// Indices
	{
		auto& indexData = result.indexData;
		indexData.reserve(12 * 3);
		{
			indexData.push_back(0);
			indexData.push_back(1);
			indexData.push_back(3);
		}
		{
			indexData.push_back(6);
			indexData.push_back(0);
			indexData.push_back(2);
		}
		{
			indexData.push_back(5);
			indexData.push_back(0);
			indexData.push_back(4);
		}
		{
			indexData.push_back(6);
			indexData.push_back(4);
			indexData.push_back(0);
		}
		{
			indexData.push_back(0);
			indexData.push_back(3);
			indexData.push_back(2);
		}
		{
			indexData.push_back(5);
			indexData.push_back(1);
			indexData.push_back(0);
		}
		{
			indexData.push_back(3);
			indexData.push_back(1);
			indexData.push_back(5);
		}
		{
			indexData.push_back(7);
			indexData.push_back(4);
			indexData.push_back(6);
		}
		{
			indexData.push_back(4);
			indexData.push_back(7);
			indexData.push_back(5);
		}
		{
			indexData.push_back(7);
			indexData.push_back(6);
			indexData.push_back(2);
		}
		{
			indexData.push_back(7);
			indexData.push_back(2);
			indexData.push_back(3);
		}
		{
			indexData.push_back(7);
			indexData.push_back(3);
			indexData.push_back(5);
		}
	}
	return result;
}

MeshData MeshCreation::CreateScreenTriangle_indexed()
{
	MeshData result;
	auto& vertexData = result.vertexData;
	vertexData.resize(3);
	
	vertexData[0].position = { -1.0f, -1.0f, 0.f };
	vertexData[1].position = { 3.f, -1.f, 0.f };
	vertexData[2].position = { -1.f, 3.f, 0.f };

	auto& indexData = result.indexData;
	indexData.reserve(3);
	
	indexData.push_back(0);
	indexData.push_back(1);
	indexData.push_back(2);

	return result;
}

std::vector<std::vector<VertextAttribute>>  MeshCreation::LoadFromFile(const char* filename)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string err;
	bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename);

	if (!err.empty()) {
		std::cout << err << std::endl;
	}

	if (!ret)
	{
		return {};
	}

	const int numShapes = shapes.size();
	std::vector<std::vector<VertextAttribute>> result;
	result.reserve(numShapes);

	// Loop over shapes
	for (int s = 0; s < numShapes; s++) {

		result.emplace_back();
		std::vector<VertextAttribute>& writeMesh = result.back();

		// Loop over faces(polygon)
		const tinyobj::mesh_t& mesh = shapes[s].mesh;
		
		size_t index_offset = 0;
		for (size_t f = 0; f < mesh.num_face_vertices.size(); f++) {
			const int faceVertices = mesh.num_face_vertices[f];

			// Loop over vertices in the face.
			for (int v = 0; v < faceVertices; v++) {
				writeMesh.emplace_back();
				VertextAttribute& vertexAttribute = writeMesh.back();

				// access to vertex
				tinyobj::index_t idx = mesh.indices[index_offset + v];

				tinyobj::real_t vx = attrib.vertices[3 * idx.vertex_index + 0];
				tinyobj::real_t vy = attrib.vertices[3 * idx.vertex_index + 1];
				tinyobj::real_t vz = attrib.vertices[3 * idx.vertex_index + 2];
				vertexAttribute.position = { vx,vy,vz };

				tinyobj::real_t nx = attrib.normals[3 * idx.normal_index + 0];
				tinyobj::real_t ny = attrib.normals[3 * idx.normal_index + 1];
				tinyobj::real_t nz = attrib.normals[3 * idx.normal_index + 2];
				vertexAttribute.normal = { nx,ny,nz };

				tinyobj::real_t tx = attrib.texcoords[2 * idx.texcoord_index + 0];
				tinyobj::real_t ty = attrib.texcoords[2 * idx.texcoord_index + 1];
				vertexAttribute.texcoord = { tx,ty };
			}
			index_offset += faceVertices;

			// per-face material
			//mesh.material_ids[f];
		}
	}

	CalcTangents(result);

	return result;
}

