#pragma once
#include "ShaderProgram.h"
#include "FrameBuffer.h"
#include "VertexArray.h"

class Texture;

class GausFilterer
{
public:
	void Init();
	void Filter(Texture& textureToFiler, Texture& tempTexture, int gausRadius);
private:
	ShaderProgram m_gausShader;
	FrameBuffer m_framebuffer;
	VertexArray m_emptyVao;

};