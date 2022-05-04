#include "Game/Player.hpp"
#include "Game/GameCommon.hpp"

#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Math/MathUtils.hpp"


//----------------------------------------------------------------------------------------------------
Player::Player( Game* owner, Vec3 const& startPosition, EulerAngles const& orientation ):
	Object( owner, startPosition, orientation )
{
	
}


//----------------------------------------------------------------------------------------------------
Player::~Player()
{

}


//----------------------------------------------------------------------------------------------------
void Player::Render() const
{

}


//----------------------------------------------------------------------------------------------------
void Player::DebugRender() const
{

}


//----------------------------------------------------------------------------------------------------
void Player::Update( float deltaSeconds )
{
	UpdateUsingKeyboard( deltaSeconds );
}


//----------------------------------------------------------------------------------------------------
Vec3 Player::GetForward() const
{
	return m_forwardVector;
}


//----------------------------------------------------------------------------------------------------
void Player::UpdateUsingKeyboard( float deltaSeconds )
{
	Mat44 orientMat = m_orientation.GetAsMatrix_XFwd_YLeft_ZUp();

	IntVec2 mouseRelativeMovement = g_theInput->GetMouseRelativeMovement();
	float mouseSenstivity = 0.02f;
	
	if ( deltaSeconds != 0.0f )
	{
		m_orientation.m_yawDegrees -= mouseRelativeMovement.x * mouseSenstivity;
		m_orientation.m_pitchDegrees += mouseRelativeMovement.y * mouseSenstivity;
	}

	m_orientation.m_rollDegrees = Clamp( m_orientation.m_rollDegrees, -45.f, 45.f );
	m_orientation.m_pitchDegrees = Clamp( m_orientation.m_pitchDegrees, -85.f, 85.f );

	GenerateDirectionVectors();
	
	float multiplier = 1.f;

	if ( g_theInput->IsKeyDown( KEYCODE_SHIFT ) )
	{
		multiplier = 10.f;
	}

	if ( g_theInput->IsKeyDown( 'W' ) )
	{
		m_position += multiplier * m_forwardVector * deltaSeconds;
	}

	if ( g_theInput->IsKeyDown( 'S' ) )
	{
		m_position -= multiplier * m_forwardVector * deltaSeconds;
	}

	if ( g_theInput->IsKeyDown( 'D' ) )
	{
		m_position -= multiplier * m_leftVector * deltaSeconds;
	}

	if ( g_theInput->IsKeyDown( 'A' ) )
	{
		m_position += multiplier * m_leftVector * deltaSeconds;
	}

	if ( g_theInput->IsKeyDown( 'Q' ) )
	{
		m_position += multiplier * ( Vec3( 0.0f, 0.0f, 1.0f ) ) * deltaSeconds;
	}

	if ( g_theInput->IsKeyDown( 'E' ) )
	{
		m_position -= multiplier * ( Vec3( 0.0f, 0.0f, 1.0f ) ) * deltaSeconds;
	}
}


//----------------------------------------------------------------------------------------------------
void Player::GenerateDirectionVectors()
{
	m_orientation.GetAsVectors_XFwd_YLeft_ZUp( m_forwardVector, m_leftVector, m_upVector );
}

