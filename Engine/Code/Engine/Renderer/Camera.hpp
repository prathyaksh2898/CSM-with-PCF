#pragma once
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/Vec3.hpp"


//-----------------------------------------------------------------------------------------------
struct AABB2;


//-----------------------------------------------------------------------------------------------
enum class CameraType
{
	ORTHOGRAPHIC,
	PERSPECTIVE,

	NUMBER
};


//-----------------------------------------------------------------------------------------------
struct Camera
{
private:
	Vec3        m_orthoBottomLeft  = Vec3::ZERO;
	Vec3        m_orthoTopRight    = Vec3::ZERO;

	EulerAngles m_orientation;
	Vec3        m_position         = Vec3::ZERO;

	float       m_fieldOfView      = 0.0f;
	float       m_aspect           = 0.0f;
	float       m_zNear            = 0.1f;
	float       m_zFar             = 1.0f;

	Mat44       m_gameSpace;

	CameraType  m_cameraType = CameraType::ORTHOGRAPHIC;

	AABB2       m_normalizedViewport = AABB2::ZEROTOONE;

public:
	void                  SetOrthoView(Vec3 const& bottomLeft, Vec3 const& topRight);
	void                  SetNormalizedViewport( AABB2 const& viewport = AABB2::ZEROTOONE );
	AABB2                 GetNormalizedViewport( );
	AABB2                 GetViewport( Vec2 const& outputSize ) const;
				          
	void                  SetCameraType( CameraType type);
		                  
	Vec3                  GetOrthoBottomLeft() const;
	Vec3                  GetOrthoTopRight() const;
	virtual Vec3          GetPosition() const;
	virtual EulerAngles   GetOrientation() const;
		  		  
	Mat44                 GetProjectionMatrix() const;
	Mat44                 GetCameraModelMatrix() const;
	Mat44                 GetCameraOrientationMatrix() const;
	virtual Mat44         GetViewMatrix() const;
	Mat44                 GetRenderMatrix() const;
		                  		        
	void                  SetGameSpace(Vec3 const& gameRight, Vec3 const& gameUp, Vec3 const& gameAway );
	void                  SetCameraPositionAndOrientation( Vec3 position, EulerAngles const& orientation );
	void                  SetFieldOfView( float fov );
	void                  SetAspect( float aspect );
	void                  SetZNearZFar( float near, float far );
		                  
	float                 GetZNear() const;
	float                 GetZFar() const;
	float                 GetAspect() const;
	float                 GetFOV() const;

	void                  Translate3D(const Vec3& translation2D);
};
