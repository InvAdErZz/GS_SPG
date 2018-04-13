#include "stdafx.h"

//MAYBE REFACTOR LATER
//#include "DynamicLight.h"
//
//void DynamicLight::Init()
//{
//	m_spotlight.SetPerspection(1.f, 1000.f, 45);
//
//	m_shadowDepthTexture.Create();
//	m_shadowDepthTexture.Bind();
//	m_shadowDepthTexture.TextureImage(0, GL_DEPTH_COMPONENT, shadowDimensions.x, shadowDimensions.y, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
//	m_shadowDepthTexture.SetNearestNeighbourFiltering();
//	m_shadowDepthTexture.SetClampBorder(glm::vec4(0.f, 0.f, 0.f, 1.f));
//	m_shadowDepthTexture.Unbind();
//
//	m_shadowFrameBuffer.Create();
//	m_shadowFrameBuffer.Bind();
//	m_shadowFrameBuffer.BindTexture(GL_DEPTH_ATTACHMENT, m_shadowDepthTexture[i].GetHandle(), 0);
//	m_shadowFrameBuffer.SetDrawBuffers(GL_NONE);
//	m_shadowFrameBuffer.SetReadBuffers(GL_NONE);
//	const GLenum framBufferStatus = m_shadowFrameBuffer[i].GetFrameBufferStatus();
//
//	assert(framBufferStatus == GL_FRAMEBUFFER_COMPLETE);
//	m_shadowFrameBuffer[i].Unbind();
//
//}
