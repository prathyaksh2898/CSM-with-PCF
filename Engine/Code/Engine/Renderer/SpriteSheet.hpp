#pragma once
#include "SpriteDefinition.hpp"
#include <vector>
#include "Engine/Math/IntVec2.hpp"

class Texture;
struct Vec2;

class SpriteSheet {
public:
	explicit SpriteSheet(Texture const& texture, IntVec2 const& simpleGridLayout);
	
	Texture const&			GetTexture() const;
	int						GetNumSprites() const;
	SpriteDefinition const& GetSpriteDefinition(int spriteIndex) const;
	SpriteDefinition const& GetSpriteDefinition(IntVec2 spriteCoords) const;
	void					GetSpriteUVs(Vec2& out_uvAtMins, Vec2& out_uvAtMaxs, int spriteIndex) const;
	void					GetSpriteUVs(Vec2& out_uvAtMins, Vec2& out_uvAtMaxs, IntVec2 spriteCoords ) const;

protected:
	Texture const&				  m_texture;
	std::vector<SpriteDefinition> m_spriteDefs;
	IntVec2                       m_layout = IntVec2(0, 0);
};