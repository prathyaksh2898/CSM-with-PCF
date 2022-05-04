#pragma once
#include "Engine/Renderer/VertexData/Vertex_PCU.hpp"
#include "Engine/Math/AABB2.hpp"
#include <string>
#include <vector>
#include "SpriteSheet.hpp"


enum class TextBoxMode{
	SHRINK,
	OVERRUN,
	NUM_MODES
};



//------------------------------------------------------------------------------------------------
class BitmapFont
{
	friend class Renderer; // Only the RenderContext can create new BitmapFont objects!

private:
	BitmapFont(char const* fontFilePathNameWithNoExtension, Texture const& fontTexture);

public:
	Texture const& GetTexture() const;

	void AddVertsForText2D(std::vector<Vertex_PCU>& vertexArray, Vec2 const& textMins,
		float cellHeight, std::string const& text, Rgba8 const& tint = Rgba8::WHITE, float cellAspect = 1.f);

	void AddVertsForTextInBox2D(std::vector<Vertex_PCU>& vertexArray, AABB2 const& box, float cellHeight,
		std::string const& text, Rgba8 const& tint = Rgba8::WHITE, float cellAspect = 1.f,
		Vec2 const& alignment = Vec2(.5f, .5f), TextBoxMode mode = TextBoxMode::SHRINK, int maxGlyphsToDraw = 99999999);


	std::string GetFontPathWithoutExtension() const;

protected:
	float GetGlyphAspect(int glyphUnicode) const;
	float GetPureTextWidth(std::string const& text) const;

protected:
	std::string	m_fontFilePathNameWithNoExtension;
	SpriteSheet	m_fontGlyphsSpriteSheet;
};
