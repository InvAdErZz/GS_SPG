#pragma once
#include "ShaderProgram.h"
#include "AttributeBuffer.h"
#include "VertexArray.h"
#include "FrameBuffer.h"
#include "LookupBuffer.h"
#include "Texture3d.h"

class ProceduralMesh
{
public:
	ProceduralMesh():m_numRockTriangles(0){}
	void Init();
	void GenerateMesh(const LookupBuffer& lookupBuffer);
	void Render();

	AttributeBuffer m_rockVertices;
	VertexArray m_rockVao;
	GLuint m_numRockTriangles;
};
