#define STB_IMAGE_IMPLEMENTATION
#include "ThirdParty/stb/stb_image.h"
#include "Image.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "ErrorWarningAssert.hpp"


//-----------------------------------------------------------------------------------------------
Image::Image(char const* imageFilePath):
	m_imageFilePath(imageFilePath)
{
	
	int imageTexelSizeX = 0; 
	int imageTexelSizeY = 0; 
	int numComponents = 0; 
	int numComponentsRequested = 0; // don't care; we support 3 (24-bit RGB) or 4 (32-bit RGBA)

	stbi_set_flip_vertically_on_load(1); // We prefer uvTexCoords has origin (0,0) at BOTTOM LEFT
	unsigned char* imageData = stbi_load(imageFilePath, &imageTexelSizeX, &imageTexelSizeY, &numComponents, numComponentsRequested);
	
	GUARANTEE_OR_DIE(imageData, Stringf("Failed to load image \"%s\"", imageFilePath));
	GUARANTEE_OR_DIE(numComponents >= 3 && numComponents <= 4 && imageTexelSizeX > 0 && imageTexelSizeY > 0, Stringf("ERROR loading image \"%s\" (Bpp=%i, size=%i,%i)", imageFilePath, numComponents, imageTexelSizeX, imageTexelSizeY));

	m_dimensions = IntVec2(imageTexelSizeX, imageTexelSizeY);
	int numTexels = imageTexelSizeX * imageTexelSizeY;

	for (int texelNum = 0; texelNum < numTexels; ++texelNum) 
	{
		
		unsigned char r = imageData[numComponents * texelNum];
		unsigned char g = imageData[numComponents * texelNum + 1];
		unsigned char b = imageData[numComponents * texelNum + 2];
		unsigned char a = 255;
		
		if (numComponents == 4) 
		{
			a = imageData[numComponents * texelNum + 3];
		}

		Rgba8 color = Rgba8(r, g, b, a);
		m_rgbaTexels.push_back(color);
	}

	m_dimensions.x = imageTexelSizeX;
	m_dimensions.y = imageTexelSizeY;

}


//-----------------------------------------------------------------------------------------------
Image::Image()
{

}


//-----------------------------------------------------------------------------------------------
std::string const& Image::GetImageFilePath() const
{
	return m_imageFilePath;
}


//-----------------------------------------------------------------------------------------------
int Image::GetBytePerPixel() const
{
	return sizeof( Rgba8 );
}

//-----------------------------------------------------------------------------------------------
IntVec2 Image::GetDimensions() const
{
	return m_dimensions;
}


//-----------------------------------------------------------------------------------------------
int Image::GetWidth() const
{
	return m_dimensions.x;
}


//-----------------------------------------------------------------------------------------------
int Image::GetHeight() const
{
	return m_dimensions.y;
}


//-----------------------------------------------------------------------------------------------
int Image::GetPitch() const
{
	return GetWidth();
}


//-----------------------------------------------------------------------------------------------
Rgba8 const* Image::GetRawData() const
{
	return m_rgbaTexels.data();
}


//-----------------------------------------------------------------------------------------------
Rgba8 Image::GetTexelColor(IntVec2 const& texelCoords) const
{
	int index = texelCoords.x * m_dimensions.y + texelCoords.y;
	return m_rgbaTexels[index];
}


//-----------------------------------------------------------------------------------------------
void Image::SetTexelColor(IntVec2 const& texelCoords, Rgba8 const& newColor)
{
	int index = texelCoords.x * m_dimensions.y + texelCoords.y;
	m_rgbaTexels[index] = newColor;
}


//-----------------------------------------------------------------------------------------------
void Image::SetupAsSolidColor( int width, int height, Rgba8 const& color )
{
	m_dimensions = IntVec2( width, height );

	for (int y = 0; y < height; y++) 
	{
		for (int x = 0; x < width; x++)
		{
			m_rgbaTexels.push_back( color );
		}
	}
}

