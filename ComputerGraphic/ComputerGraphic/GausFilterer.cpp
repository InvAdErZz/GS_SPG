#include "stdafx.h"
#include "GausFilterer.h"
#include "Texture.h"

namespace
{
	const std::string INPUT_TEX_UNIFORM_NAME("inputTex");
	const std::string GAUS_RADIUS_NAME("gausRadius");
	const std::string IS_HORIZONTAL_UNIFORM_NAME("isHorizontal");
}


void GausFilterer::Init()
{
	if (!m_gausShader.CreateShaders("../Shader/blurShader.vert", "../Shader/blurShader.frag"))
	{
		assert(false);
	}

	m_gausShader.LinkShaders();
	m_gausShader.UseProgram();
	m_gausShader.FindUniforms({ INPUT_TEX_UNIFORM_NAME, GAUS_RADIUS_NAME,IS_HORIZONTAL_UNIFORM_NAME });
	m_framebuffer.Create();
	m_emptyVao.Create();
	m_gausShader.UnuseProgram();
}

void GausFilterer::Filter(Texture& textureToFiler, Texture& tempTexture, int gausRadius)
{
	m_framebuffer.Bind();
	m_gausShader.UseProgram();
	m_emptyVao.Bind();

	constexpr int InputTextureUnit = 0;
	m_gausShader.SetSamplerTextureUnit(InputTextureUnit, INPUT_TEX_UNIFORM_NAME);
	m_gausShader.SetIntUniform(gausRadius, GAUS_RADIUS_NAME);

	{
		m_framebuffer.BindTexture(GL_COLOR_ATTACHMENT0, tempTexture.GetHandle(), 0);
		m_gausShader.SetBoolUniform(false, IS_HORIZONTAL_UNIFORM_NAME);
		textureToFiler.BindToTextureUnit(InputTextureUnit);
		glClear(GL_COLOR_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}
	{
		m_framebuffer.BindTexture(GL_COLOR_ATTACHMENT0, textureToFiler.GetHandle(), 0);
		m_gausShader.SetBoolUniform(true, IS_HORIZONTAL_UNIFORM_NAME);
		tempTexture.BindToTextureUnit(InputTextureUnit);
		glClear(GL_COLOR_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}

	m_emptyVao.Unbind();
	m_gausShader.UnuseProgram();
	m_framebuffer.Unbind();
}
