#pragma once
#include "stb_image.h"
#include <memory>

struct Stb_Image_Deleter
{
	void operator()(stbi_uc * imageData)
	{	
		stbi_image_free(imageData);	
	}
};

class Image
{
public:
	void LoadImage(const char* file);

	const void* GetData() const { return m_imageData.get(); }
	int GetWidth() const { return m_width; }
	int GetHeight() const { return m_height; }
	int GetNumberChannels() const { return m_numberChannels; }

private:
	std::unique_ptr<stbi_uc, Stb_Image_Deleter> m_imageData;

	int m_width;
	int m_height;
	int m_numberChannels;	

};
