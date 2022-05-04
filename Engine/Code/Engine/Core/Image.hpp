#pragma once
#include <string>
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include <vector>


//-----------------------------------------------------------------------------------------------
class Image
{
public:
	                      Image(char const* imageFilePath);
						  Image();
					      
	std::string const&    GetImageFilePath() const;
	
	int                   GetBytePerPixel() const;
	IntVec2		          GetDimensions() const;
	int                   GetWidth() const;
	int                   GetHeight() const;
	int                   GetPitch() const;

	Rgba8 const*		  GetRawData() const;

	Rgba8			      GetTexelColor(IntVec2 const& texelCoords) const;
	void			      SetTexelColor(IntVec2 const& texelCoords, Rgba8 const& newColor);

	void                  SetupAsSolidColor( int width, int height, Rgba8 const& color );

private:
	std::string			  m_imageFilePath;
	IntVec2			      m_dimensions = IntVec2(0, 0);
	std::vector< Rgba8 >  m_rgbaTexels;

};
