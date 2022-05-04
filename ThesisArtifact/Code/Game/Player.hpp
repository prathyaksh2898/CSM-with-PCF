#pragma once
#include "Game/Object.hpp"


//----------------------------------------------------------------------------------------------------
class Player : public Object
{
public:
	Player( Game* owner, Vec3 const& startPosition, EulerAngles const& orientation );
	~Player();

	void Render() const override;
	void DebugRender() const override;
	void Update( float deltaSeconds ) override;

	Vec3 GetForward() const;

protected:
	void UpdateUsingKeyboard( float deltaSeconds );
	void GenerateDirectionVectors();

protected:
	Vec3 m_forwardVector = Vec3( 1.0f, 0.0f, 0.0f);
	Vec3 m_leftVector    = Vec3( 0.0f, 1.0f, 0.0f );
	Vec3 m_upVector      = Vec3( 0.0f, 0.0f, 1.0f );
};