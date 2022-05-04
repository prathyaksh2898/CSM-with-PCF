#include "Object.hpp"

#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"

#include "Engine/Math/MathUtils.hpp"


//-----------------------------------------------------------------------------------------------
Object::Object(Game* Owner, Vec3 const& startPosition, EulerAngles const& angles) :
	m_game(Owner), m_position(startPosition), m_orientation(angles)
{

}


//-----------------------------------------------------------------------------------------------
Object::~Object() 
{

}


//-----------------------------------------------------------------------------------------------
void Object::DebugRender() const
{
	
}


//-----------------------------------------------------------------------------------------------
void Object::Render() const 
{

}
