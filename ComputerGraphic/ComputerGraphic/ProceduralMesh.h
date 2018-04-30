#pragma once
#include "ShaderProgram.h"
#include "AttributeBuffer.h"
#include "VertexArray.h"
#include "FrameBuffer.h"
#include "LookupBuffer.h"
#include "Texture3d.h"

struct ProceduralMeshVertex
{
	static constexpr int PositionLocation = 0;
	static constexpr int NormalLocation = 1;

	static constexpr const char* POSITION_ATTRIBUTE_NAME = "in_Position";
	static constexpr const char* NORMAL_ATTRIBUTE_NAME = "in_Normal";

	glm::vec3 position;
	glm::vec3 normal;
};

class ProceduralMesh
{
public:
	ProceduralMesh():m_numRockTriangles(0){}
	void Init();
	void GenerateMesh(const LookupBuffer& lookupBuffer, float baseDensity);
	void Render();
	VertexArray emptyVao;

	AttributeBuffer m_rockVertices;
	VertexArray m_rockVao;
	GLuint m_numRockTriangles;
};
