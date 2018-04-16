#pragma once
#include <glm.hpp>
#include "AttributeBuffer.h"

class ShaderProgram;
class LookupBuffer
{
public:
	void WriteLookupTablesToGpu();
	void UpdateUniforms(ShaderProgram& shader) const;

protected:
	AttributeBuffer m_edgeBuffer;
	AttributeBuffer m_triBuffer;
};
