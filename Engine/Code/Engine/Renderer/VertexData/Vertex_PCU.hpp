#pragma once
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"

struct Vertex_PCU
{
public:
	Vec3 m_position;
	Rgba8 m_color;
	Vec2 m_uvTexCoords;

public:
	// Construction/Destruction
	~Vertex_PCU() {}												// destructor (do nothing)
	Vertex_PCU() {}												// default constructor (do nothing)
	explicit Vertex_PCU(Vec3 const& position, Rgba8 const& tint, Vec2 const& uvTexCoords);		

	//static buffer_attribute_t const LAYOUT[];
};
