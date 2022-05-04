#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/Camera.hpp"

//------------------------------------------------------------------------------------------------
Vec3 Camera::GetOrthoBottomLeft() const
{
	return m_orthoBottomLeft;
}


//------------------------------------------------------------------------------------------------
Vec3 Camera::GetOrthoTopRight() const
{
	return m_orthoTopRight;
}


//------------------------------------------------------------------------------------------------
Vec3 Camera::GetPosition() const
{
	return m_position;
}


//------------------------------------------------------------------------------------------------
EulerAngles Camera::GetOrientation() const
{
	return m_orientation;
}


//------------------------------------------------------------------------------------------------
Mat44 Camera::GetProjectionMatrix() const
{
	Mat44 matrix;

	if ( m_cameraType == CameraType::ORTHOGRAPHIC )
		return Mat44::CreateOrthoProjection( m_orthoBottomLeft.x, m_orthoTopRight.x, m_orthoBottomLeft.y, m_orthoTopRight.y, m_orthoBottomLeft.z, m_orthoTopRight.z );

	else if ( m_cameraType == CameraType::PERSPECTIVE )
		return Mat44::CreatePerspectiveProjection( m_fieldOfView, m_aspect, m_zNear, m_zFar );

	return matrix;
}


//------------------------------------------------------------------------------------------------
Mat44 Camera::GetCameraModelMatrix() const
{
	Mat44 modelMatrix;

	modelMatrix.AppendTranslation3D( m_position );
	modelMatrix.Append( m_orientation.GetAsMatrix_XFwd_YLeft_ZUp() );

	return modelMatrix;
}


//------------------------------------------------------------------------------------------------
Mat44 Camera::GetCameraOrientationMatrix() const
{
	Mat44 modelMatrix;
	modelMatrix.Append( m_orientation.GetAsMatrix_XFwd_YLeft_ZUp() );

	return modelMatrix;
}


//------------------------------------------------------------------------------------------------
Mat44 Camera::GetViewMatrix() const
{
	Mat44 viewMatrix;

	viewMatrix.AppendTranslation3D( m_position );
	viewMatrix.Append( m_orientation.GetAsMatrix_XFwd_YLeft_ZUp() );

	viewMatrix = viewMatrix.GetOrthonormalInverse();

	return viewMatrix;
}


//------------------------------------------------------------------------------------------------
Mat44 Camera::GetRenderMatrix() const
{
	return m_gameSpace;
}


//------------------------------------------------------------------------------------------------
void Camera::SetGameSpace( Vec3 const& gameRight, Vec3 const& gameUp, Vec3 const& gameAway )
{
	m_gameSpace.m_values[ Mat44::Ix ] = gameRight.x;
	m_gameSpace.m_values[ Mat44::Iy ] = gameRight.y;
	m_gameSpace.m_values[ Mat44::Iz ] = gameRight.z;

	m_gameSpace.m_values[ Mat44::Jx ] = gameUp.x;
	m_gameSpace.m_values[ Mat44::Jy ] = gameUp.y;
	m_gameSpace.m_values[ Mat44::Jz ] = gameUp.z;

	m_gameSpace.m_values[ Mat44::Kx ] = gameAway.x;
	m_gameSpace.m_values[ Mat44::Ky ] = gameAway.y;
	m_gameSpace.m_values[ Mat44::Kz ] = gameAway.z;

	m_gameSpace = m_gameSpace.GetOrthonormalInverse();
}


//------------------------------------------------------------------------------------------------
void Camera::SetCameraPositionAndOrientation( Vec3 position, EulerAngles const& orientation )
{
	m_position = position;
	m_orientation = orientation;
}


//------------------------------------------------------------------------------------------------
void Camera::SetFieldOfView( float fov )
{
	m_fieldOfView = fov;
}


//------------------------------------------------------------------------------------------------
void Camera::SetAspect( float aspect )
{
	m_aspect = aspect;
}


//------------------------------------------------------------------------------------------------
void Camera::SetZNearZFar( float near, float far )
{
	m_zNear = near;
	m_zFar  = far;
}


//------------------------------------------------------------------------------------------------
float Camera::GetZNear() const
{
	return m_zNear;
}


//------------------------------------------------------------------------------------------------
float Camera::GetZFar() const
{
	return m_zFar;
}


//------------------------------------------------------------------------------------------------
float Camera::GetAspect() const
{
	return m_aspect;
}


//------------------------------------------------------------------------------------------------
float Camera::GetFOV() const
{
	return m_fieldOfView;
}


//------------------------------------------------------------------------------------------------
void Camera::SetOrthoView( Vec3 const& bottomLeft, Vec3 const& topRight )
{
	m_orthoBottomLeft = bottomLeft;
	m_orthoTopRight = topRight;
}


//------------------------------------------------------------------------------------------------
void Camera::SetNormalizedViewport( AABB2 const& viewport )
{
	m_normalizedViewport = viewport;
}


//------------------------------------------------------------------------------------------------
AABB2 Camera::GetNormalizedViewport()
{
	return m_normalizedViewport;
}


//------------------------------------------------------------------------------------------------
AABB2 Camera::GetViewport( Vec2 const& outputSize ) const
{
	Vec2 newMins = m_normalizedViewport.m_mins * outputSize;
	Vec2 newMaxs = m_normalizedViewport.m_maxs * outputSize;

	//newMins.y = outputSize.y - newMins.y;
	//newMaxs.y = outputSize.y - newMaxs.y;

	return AABB2( newMins, newMaxs );
}


//------------------------------------------------------------------------------------------------
void Camera::SetCameraType( CameraType type )
{
	m_cameraType = type;
}


//------------------------------------------------------------------------------------------------
void Camera::Translate3D( const Vec3& translation3D )
{
	m_orthoBottomLeft += translation3D;
	m_orthoTopRight += translation3D;
}