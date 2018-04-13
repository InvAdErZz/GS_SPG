#include "stdafx.h"
#include "Image.h"

void Image::LoadImage(const char * file)
{
	m_imageData.reset(stbi_load(file, &m_width, &m_height, &m_numberChannels, 0));
	if (!m_imageData)
	{
		std::printf("failure Loading Texture!!!");
		assert(false);
	}
}
