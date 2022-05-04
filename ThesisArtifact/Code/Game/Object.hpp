#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Renderer/VertexData/Vertex_PCU.hpp"


//-----------------------------------------------------------------------------------------------
class Game;


//-----------------------------------------------------------------------------------------------
class Object
{
public:
	Object( Game* Owner, Vec3 const& startPosition, EulerAngles const& orientation );
	virtual ~Object();

	virtual void Update( float deltaseconds ) = 0;
	virtual void Render() const;
	virtual void DebugRender() const;

public:
	Vec3        m_position;
	EulerAngles m_orientation;

	Vec3        m_velocity;
	EulerAngles m_angularVelocity;

	Mat44       m_modelMatrix;

	Game*       m_game = nullptr;
};