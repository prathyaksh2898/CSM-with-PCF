#include "Mat44.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/EngineCommon.hpp"

//-----------------------------------------------------------------------------------------------
Mat44::Mat44()
{
	m_values[ Ix ] = 1.0f;
	m_values[ Iy ] = 0.0f;
	m_values[ Iz ] = 0.0f;
	m_values[ Iw ] = 0.0f;

	m_values[ Jx ] = 0.0f;
	m_values[ Jy ] = 1.0f;
	m_values[ Jz ] = 0.0f;
	m_values[ Jw ] = 0.0f;

	m_values[ Kx ] = 0.0f;
	m_values[ Ky ] = 0.0f;
	m_values[ Kz ] = 1.0f;
	m_values[ Kw ] = 0.0f;

	m_values[ Tx ] = 0.0f;
	m_values[ Ty ] = 0.0f;
	m_values[ Tz ] = 0.0f;
	m_values[ Tw ] = 1.0f;
}


//-----------------------------------------------------------------------------------------------
Mat44::Mat44( Vec2 const& iBasis2D, Vec2 const& jBasis2D, Vec2 const& translation2D )
{
	m_values[ Ix ] = iBasis2D.x;
	m_values[ Iy ] = iBasis2D.y;
	m_values[ Iz ] = 0.0f;
	m_values[ Iw ] = 0.0f;

	m_values[ Jx ] = jBasis2D.x;
	m_values[ Jy ] = jBasis2D.y;
	m_values[ Jz ] = 0.0f;
	m_values[ Jw ] = 0.0f;

	m_values[ Kx ] = 0.0f;
	m_values[ Ky ] = 0.0f;
	m_values[ Kz ] = 1.0f;
	m_values[ Kw ] = 0.0f;

	m_values[ Tx ] = translation2D.x;
	m_values[ Ty ] = translation2D.y;
	m_values[ Tz ] = 0.0f;
	m_values[ Tw ] = 1.f;
}


//-----------------------------------------------------------------------------------------------
Mat44::Mat44( Vec3 const& iBasis3D, Vec3 const& jBasis3D, Vec3 const& kBasis3D, Vec3 const& translation3D )
{
	m_values[ Ix ] = iBasis3D.x;
	m_values[ Iy ] = iBasis3D.y;
	m_values[ Iz ] = iBasis3D.z;
	m_values[ Iw ] = 0.0f;

	m_values[ Jx ] = jBasis3D.x;
	m_values[ Jy ] = jBasis3D.y;
	m_values[ Jz ] = jBasis3D.z;
	m_values[ Jw ] = 0.0f;

	m_values[ Kx ] = kBasis3D.x;
	m_values[ Ky ] = kBasis3D.y;
	m_values[ Kz ] = kBasis3D.z;
	m_values[ Kw ] = 0.0f;

	m_values[ Tx ] = translation3D.x;
	m_values[ Ty ] = translation3D.y;
	m_values[ Tz ] = translation3D.z;
	m_values[ Tw ] = 1.f;

}


//-----------------------------------------------------------------------------------------------
Mat44::Mat44( Vec4 const& iBasis4D, Vec4 const& jBasis4D, Vec4 const& kBasis4D, Vec4 const& translation4D )
{
	m_values[ Ix ] = iBasis4D.x;
	m_values[ Iy ] = iBasis4D.y;
	m_values[ Iz ] = iBasis4D.z;
	m_values[ Iw ] = iBasis4D.w;

	m_values[ Jx ] = jBasis4D.x;
	m_values[ Jy ] = jBasis4D.y;
	m_values[ Jz ] = jBasis4D.z;
	m_values[ Jw ] = jBasis4D.w;

	m_values[ Kx ] = kBasis4D.x;
	m_values[ Ky ] = kBasis4D.y;
	m_values[ Kz ] = kBasis4D.z;
	m_values[ Kw ] = kBasis4D.w;

	m_values[ Tx ] = translation4D.x;
	m_values[ Ty ] = translation4D.y;
	m_values[ Tz ] = translation4D.z;
	m_values[ Tw ] = translation4D.w;
}


//-----------------------------------------------------------------------------------------------
Mat44::Mat44( float const* sixteenValuesBasisMajor )
{
	m_values[ Ix ] = sixteenValuesBasisMajor[ Ix ];
	m_values[ Iy ] = sixteenValuesBasisMajor[ Iy ];
	m_values[ Iz ] = sixteenValuesBasisMajor[ Iz ];
	m_values[ Iw ] = sixteenValuesBasisMajor[ Iw ];

	m_values[ Jx ] = sixteenValuesBasisMajor[ Jx ];
	m_values[ Jy ] = sixteenValuesBasisMajor[ Jy ];
	m_values[ Jz ] = sixteenValuesBasisMajor[ Jz ];
	m_values[ Jw ] = sixteenValuesBasisMajor[ Jw ];

	m_values[ Kx ] = sixteenValuesBasisMajor[ Kx ];
	m_values[ Ky ] = sixteenValuesBasisMajor[ Ky ];
	m_values[ Kz ] = sixteenValuesBasisMajor[ Kz ];
	m_values[ Kw ] = sixteenValuesBasisMajor[ Kw ];

	m_values[ Tx ] = sixteenValuesBasisMajor[ Tx ];
	m_values[ Ty ] = sixteenValuesBasisMajor[ Ty ];
	m_values[ Tz ] = sixteenValuesBasisMajor[ Tz ];
	m_values[ Tw ] = sixteenValuesBasisMajor[ Tw ];
}


//-----------------------------------------------------------------------------------------------
Mat44 const Mat44::CreateTranslation2D( Vec2 const& translationXY )
{
	Mat44 translationXY44;
	translationXY44.m_values[ Tx ] = translationXY.x;
	translationXY44.m_values[ Ty ] = translationXY.y;

	return translationXY44;
}


//-----------------------------------------------------------------------------------------------
Mat44 const Mat44::CreateTranslation3D( Vec3 const& translationXYZ )
{
	Mat44 translationXYZ44;
	translationXYZ44.m_values[ Tx ] = translationXYZ.x;
	translationXYZ44.m_values[ Ty ] = translationXYZ.y;
	translationXYZ44.m_values[ Tz ] = translationXYZ.z;

	return translationXYZ44;
}


//-----------------------------------------------------------------------------------------------
Mat44 const Mat44::CreateUniformScale2D( float uniformScaleXY )
{
	Mat44 uniformScaleXY44;
	uniformScaleXY44.m_values[ Ix ] *= uniformScaleXY;
	uniformScaleXY44.m_values[ Jy ] *= uniformScaleXY;

	return uniformScaleXY44;
}


//-----------------------------------------------------------------------------------------------
Mat44 const Mat44::CreateUniformScale3D( float uniformScaleXYZ )
{
	Mat44 uniformScaleXYZ44;
	uniformScaleXYZ44.m_values[ Ix ] *= uniformScaleXYZ;
	uniformScaleXYZ44.m_values[ Jy ] *= uniformScaleXYZ;
	uniformScaleXYZ44.m_values[ Kz ] *= uniformScaleXYZ;

	return uniformScaleXYZ44;
}


//-----------------------------------------------------------------------------------------------
Mat44 const Mat44::CreateNonUniformScale2D( Vec2 const& nonUniformScaleXY )
{
	Mat44 nonUniformScaleXY44;
	nonUniformScaleXY44.m_values[ Ix ] *= nonUniformScaleXY.x;
	nonUniformScaleXY44.m_values[ Jy ] *= nonUniformScaleXY.y;

	return nonUniformScaleXY44;
}


//-----------------------------------------------------------------------------------------------
Mat44 const Mat44::CreateNonUniformScale3D( Vec3 const& nonUniformScaleXYZ )
{
	Mat44 nonUniformScaleXYZ44;
	nonUniformScaleXYZ44.m_values[ Ix ] *= nonUniformScaleXYZ.x;
	nonUniformScaleXYZ44.m_values[ Jy ] *= nonUniformScaleXYZ.y;
	nonUniformScaleXYZ44.m_values[ Kz ] *= nonUniformScaleXYZ.z;

	return nonUniformScaleXYZ44;
}


//-----------------------------------------------------------------------------------------------
Mat44 const Mat44::CreateZRotationDegrees( float rotationDegreesAboutZ )
{
	Mat44 rotationAboutZ44;
	rotationAboutZ44.m_values[ Ix ] = CosDegrees( rotationDegreesAboutZ );
	rotationAboutZ44.m_values[ Iy ] = SinDegrees( rotationDegreesAboutZ );

	rotationAboutZ44.m_values[ Jx ] = -SinDegrees( rotationDegreesAboutZ );
	rotationAboutZ44.m_values[ Jy ] = CosDegrees( rotationDegreesAboutZ );

	return rotationAboutZ44;
}


//-----------------------------------------------------------------------------------------------
Mat44 const Mat44::CreateYRotationDegrees( float rotationDegreesAboutY )
{
	Mat44 rotationAboutY44;
	rotationAboutY44.m_values[ Ix ] = CosDegrees( rotationDegreesAboutY );
	rotationAboutY44.m_values[ Iz ] = -SinDegrees( rotationDegreesAboutY );

	rotationAboutY44.m_values[ Kx ] = SinDegrees( rotationDegreesAboutY );
	rotationAboutY44.m_values[ Kz ] = CosDegrees( rotationDegreesAboutY );

	return rotationAboutY44;
}


//-----------------------------------------------------------------------------------------------
Mat44 const Mat44::CreateXRotationDegrees( float rotationDegreesAboutX )
{
	Mat44 rotationAboutX44;
	rotationAboutX44.m_values[ Jy ] = CosDegrees( rotationDegreesAboutX );
	rotationAboutX44.m_values[ Jz ] = SinDegrees( rotationDegreesAboutX );

	rotationAboutX44.m_values[ Ky ] = -SinDegrees( rotationDegreesAboutX );
	rotationAboutX44.m_values[ Kz ] = CosDegrees( rotationDegreesAboutX );

	return rotationAboutX44;
}


//-----------------------------------------------------------------------------------------------
Vec2 const Mat44::TransformVectorQuantity2D( Vec2 const& vectorQuantityXY ) const
{
	Vec4 quantity;
	quantity.x = vectorQuantityXY.x;
	quantity.y = vectorQuantityXY.y;

	Vec4 result = TransformHomogeneous3D( quantity );
	return Vec2( result.x, result.y );
}


//-----------------------------------------------------------------------------------------------
Vec3 const Mat44::TransformVectorQuantity3D( Vec3 const& vectorQuantityXYZ ) const
{
	Vec4 quantity;
	quantity.x = vectorQuantityXYZ.x;
	quantity.y = vectorQuantityXYZ.y;
	quantity.z = vectorQuantityXYZ.z;

	Vec4 result = TransformHomogeneous3D( quantity );
	return Vec3( result.x, result.y, result.z );
}


//-----------------------------------------------------------------------------------------------
Vec2 const Mat44::TransformPosition2D( Vec2 const& positionXY ) const
{
	Vec4 quantity;
	quantity.x = positionXY.x;
	quantity.y = positionXY.y;
	quantity.w = 1.0f;

	Vec4 result = TransformHomogeneous3D( quantity );
	return Vec2( result.x, result.y );
}


//-----------------------------------------------------------------------------------------------
Vec3 const Mat44::TransformPosition3D( Vec3 const& positionXYZ ) const
{
	Vec4 quantity;
	quantity.x = positionXYZ.x;
	quantity.y = positionXYZ.y;
	quantity.z = positionXYZ.z;
	quantity.w = 1.0f;

	Vec4 result = TransformHomogeneous3D( quantity );
	return Vec3( result.x, result.y, result.z );
}


//-----------------------------------------------------------------------------------------------
Vec4 const Mat44::TransformHomogeneous3D( Vec4 const& homogeneousPoint3D ) const
{
	Vec4 quantity = homogeneousPoint3D;

	Vec4 result;
	result.x = m_values[ Ix ] * quantity.x + m_values[ Jx ] * quantity.y + m_values[ Kx ] * quantity.z + m_values[ Tx ] * quantity.w;
	result.y = m_values[ Iy ] * quantity.x + m_values[ Jy ] * quantity.y + m_values[ Ky ] * quantity.z + m_values[ Ty ] * quantity.w;
	result.z = m_values[ Iz ] * quantity.x + m_values[ Jz ] * quantity.y + m_values[ Kz ] * quantity.z + m_values[ Tz ] * quantity.w;
	result.w = m_values[ Iw ] * quantity.x + m_values[ Jw ] * quantity.y + m_values[ Kw ] * quantity.z + m_values[ Tw ] * quantity.w;

	return result;
}


//-----------------------------------------------------------------------------------------------
float* Mat44::GetAsFloatArray()
{
	return &m_values[ 0 ];
}


//-----------------------------------------------------------------------------------------------
float const* Mat44::GetAsFloatArray() const
{
	return &m_values[ 0 ];
}


//-----------------------------------------------------------------------------------------------
Vec2 const Mat44::GetIBasis2D() const
{
	Vec2 iBasis;
	iBasis.x = m_values[ Ix ];
	iBasis.y = m_values[ Iy ];
	return iBasis;
}


//-----------------------------------------------------------------------------------------------
Vec2 const Mat44::GetJBasis2D() const
{
	Vec2 jBasis;
	jBasis.x = m_values[ Jx ];
	jBasis.y = m_values[ Jy ];
	return jBasis;
}


//-----------------------------------------------------------------------------------------------
Vec2 const Mat44::GetTranslation2D() const
{
	Vec2 translation2D;
	translation2D.x = m_values[ Tx ];
	translation2D.y = m_values[ Ty ];
	return translation2D;
}


//-----------------------------------------------------------------------------------------------
Vec3 const Mat44::GetIBasis3D() const
{
	Vec3 iBasis;
	iBasis.x = m_values[ Ix ];
	iBasis.y = m_values[ Iy ];
	iBasis.z = m_values[ Iz ];
	return iBasis;
}


//-----------------------------------------------------------------------------------------------
Vec3 const Mat44::GetJBasis3D() const
{
	Vec3 jBasis;
	jBasis.x = m_values[ Jx ];
	jBasis.y = m_values[ Jy ];
	jBasis.z = m_values[ Jz ];
	return jBasis;
}


//-----------------------------------------------------------------------------------------------
Vec3 const Mat44::GetKBasis3D() const
{
	Vec3 kBasis;
	kBasis.x = m_values[ Kx ];
	kBasis.y = m_values[ Ky ];
	kBasis.z = m_values[ Kz ];
	return kBasis;
}


//-----------------------------------------------------------------------------------------------
Vec3 const Mat44::GetTranslation3D() const
{
	Vec3 translationXYZ;
	translationXYZ.x = m_values[ Tx ];
	translationXYZ.y = m_values[ Ty ];
	translationXYZ.z = m_values[ Tz ];
	return translationXYZ;
}


//-----------------------------------------------------------------------------------------------
Vec4 const Mat44::GetIBasis4D() const
{
	Vec4 iBasis;
	iBasis.x = m_values[ Ix ];
	iBasis.y = m_values[ Iy ];
	iBasis.z = m_values[ Iz ];
	iBasis.w = m_values[ Iw ];
	return iBasis;
}


//-----------------------------------------------------------------------------------------------
Vec4 const Mat44::GetJBasis4D() const
{
	Vec4 jBasis;
	jBasis.x = m_values[ Jx ];
	jBasis.y = m_values[ Jy ];
	jBasis.z = m_values[ Jz ];
	jBasis.w = m_values[ Jw ];
	return jBasis;
}


//-----------------------------------------------------------------------------------------------
Vec4 const Mat44::GetKBasis4D() const
{
	Vec4 kBasis;
	kBasis.x = m_values[ Kx ];
	kBasis.y = m_values[ Ky ];
	kBasis.z = m_values[ Kz ];
	kBasis.w = m_values[ Kw ];
	return kBasis;
}


//-----------------------------------------------------------------------------------------------
Vec4 const Mat44::GetTranslation4D() const
{
	Vec4 translationXYZW;
	translationXYZW.x = m_values[ Tx ];
	translationXYZW.y = m_values[ Ty ];
	translationXYZW.z = m_values[ Tz ];
	translationXYZW.w = m_values[ Tw ];
	return translationXYZW;
}


//-----------------------------------------------------------------------------------------------
Mat44 const Mat44::GetMatrixWithoutTranslation() const
{
	Mat44 withoutTranslation;

	withoutTranslation.m_values[ Ix ] = m_values[ Ix ];
	withoutTranslation.m_values[ Iy ] = m_values[ Iy ];
	withoutTranslation.m_values[ Iz ] = m_values[ Iz ];
	withoutTranslation.m_values[ Iw ] = m_values[ Iw ];

	withoutTranslation.m_values[ Jx ] = m_values[ Jx ];
	withoutTranslation.m_values[ Jy ] = m_values[ Jy ];
	withoutTranslation.m_values[ Jz ] = m_values[ Jz ];
	withoutTranslation.m_values[ Jw ] = m_values[ Jw ];

	withoutTranslation.m_values[ Kx ] = m_values[ Kx ];
	withoutTranslation.m_values[ Ky ] = m_values[ Ky ];
	withoutTranslation.m_values[ Kz ] = m_values[ Kz ];
	withoutTranslation.m_values[ Kw ] = m_values[ Kw ];

	return withoutTranslation;
}


//-----------------------------------------------------------------------------------------------
Mat44 const Mat44::GetTranspose() const
{
	Mat44 transposeMatrix;

	transposeMatrix.m_values[ Ix ] = m_values[ Ix ];
	transposeMatrix.m_values[ Iy ] = m_values[ Jx ];
	transposeMatrix.m_values[ Iz ] = m_values[ Kx ];
	transposeMatrix.m_values[ Iw ] = m_values[ Tx ];

	transposeMatrix.m_values[ Jx ] = m_values[ Iy ];
	transposeMatrix.m_values[ Jy ] = m_values[ Jy ];
	transposeMatrix.m_values[ Jz ] = m_values[ Ky ];
	transposeMatrix.m_values[ Jw ] = m_values[ Ty ];

	transposeMatrix.m_values[ Kx ] = m_values[ Iz ];
	transposeMatrix.m_values[ Ky ] = m_values[ Jz ];
	transposeMatrix.m_values[ Kz ] = m_values[ Kz ];
	transposeMatrix.m_values[ Kw ] = m_values[ Tz ];

	transposeMatrix.m_values[ Tx ] = m_values[ Iw ];
	transposeMatrix.m_values[ Ty ] = m_values[ Jw ];
	transposeMatrix.m_values[ Tz ] = m_values[ Kw ];
	transposeMatrix.m_values[ Tw ] = m_values[ Tw ];

	return transposeMatrix;
}


//-----------------------------------------------------------------------------------------------
Mat44 const Mat44::GetOrthonormalInverse() const
{
	Mat44 matrixWithoutTranslation = GetMatrixWithoutTranslation();
	Mat44 transposeMatrix = matrixWithoutTranslation.GetTranspose();

	Mat44 translationUndo;
	translationUndo.m_values[ Tx ] = -m_values[ Tx ];
	translationUndo.m_values[ Ty ] = -m_values[ Ty ];
	translationUndo.m_values[ Tz ] = -m_values[ Tz ];

	transposeMatrix.Append( translationUndo );

	return transposeMatrix;
}


//-----------------------------------------------------------------------------------------------
void Mat44::SetTranslation2D( Vec2 const& translationXY )
{
	m_values[ Tx ] = translationXY.x;
	m_values[ Ty ] = translationXY.y;
	m_values[ Tz ] = 0.0f;
	m_values[ Tw ] = 1.0f;
}


//-----------------------------------------------------------------------------------------------
void Mat44::SetTranslation3D( Vec3 const& translationXYZ )
{
	m_values[ Tx ] = translationXYZ.x;
	m_values[ Ty ] = translationXYZ.y;
	m_values[ Tz ] = translationXYZ.z;
	m_values[ Tw ] = 1.0f;
}


//-----------------------------------------------------------------------------------------------
void Mat44::SetIJ2D( Vec2 const& iBasis2D, Vec2 const& jBasis2D )
{
	m_values[ Ix ] = iBasis2D.x;
	m_values[ Iy ] = iBasis2D.y;
	m_values[ Iz ] = 0.0f;
	m_values[ Iw ] = 0.0f;

	m_values[ Jx ] = jBasis2D.x;
	m_values[ Jy ] = jBasis2D.y;
	m_values[ Jz ] = 0.0f;
	m_values[ Jw ] = 0.0f;
}


//-----------------------------------------------------------------------------------------------
void Mat44::SetIJT2D( Vec2 const& iBasis2D, Vec2 const& jBasis2D, Vec2 const& translationXY )
{
	SetIJ2D( iBasis2D, jBasis2D );
	SetTranslation2D( translationXY );
}


//-----------------------------------------------------------------------------------------------
void Mat44::SetIJK3D( Vec3 const& iBasis3D, Vec3 const& jBasis3D, Vec3 const& kBasis3D )
{
	m_values[ Ix ] = iBasis3D.x;
	m_values[ Iy ] = iBasis3D.y;
	m_values[ Iz ] = iBasis3D.z;
	m_values[ Iw ] = 0.0f;

	m_values[ Jx ] = jBasis3D.x;
	m_values[ Jy ] = jBasis3D.y;
	m_values[ Jz ] = jBasis3D.z;
	m_values[ Jw ] = 0.0f;

	m_values[ Kx ] = kBasis3D.x;
	m_values[ Ky ] = kBasis3D.y;
	m_values[ Kz ] = kBasis3D.z;
	m_values[ Kw ] = 0.0f;
}


//-----------------------------------------------------------------------------------------------
void Mat44::SetIJKT3D( Vec3 const& iBasis3D, Vec3 const& jBasis3D, Vec3 const& kBasis3D, Vec3 const& translationXYZ )
{
	SetIJK3D( iBasis3D, jBasis3D, kBasis3D );
	SetTranslation3D( translationXYZ );
}


//-----------------------------------------------------------------------------------------------
void Mat44::SetIJKT4D( Vec4 const& iBasis4D, Vec4 const& jBasis4D, Vec4 const& kBasis4D, Vec4 const& translationXYZW )
{
	m_values[ Ix ] = iBasis4D.x;
	m_values[ Iy ] = iBasis4D.y;
	m_values[ Iz ] = iBasis4D.z;
	m_values[ Iw ] = iBasis4D.w;

	m_values[ Jx ] = jBasis4D.x;
	m_values[ Jy ] = jBasis4D.y;
	m_values[ Jz ] = jBasis4D.z;
	m_values[ Jw ] = jBasis4D.w;

	m_values[ Kx ] = kBasis4D.x;
	m_values[ Ky ] = kBasis4D.y;
	m_values[ Kz ] = kBasis4D.z;
	m_values[ Kw ] = kBasis4D.w;

	m_values[ Tx ] = translationXYZW.x;
	m_values[ Ty ] = translationXYZW.y;
	m_values[ Tz ] = translationXYZW.z;
	m_values[ Tw ] = translationXYZW.w;
}


//-----------------------------------------------------------------------------------------------
void Mat44::Append( Mat44 const& appendThis )
{
	float result[ 16 ];

	result[ Ix ] = m_values[ Ix ] * appendThis.m_values[ Ix ] + m_values[ Jx ] * appendThis.m_values[ Iy ] + m_values[ Kx ] * appendThis.m_values[ Iz ] + m_values[ Tx ] * appendThis.m_values[ Iw ];
	result[ Iy ] = m_values[ Iy ] * appendThis.m_values[ Ix ] + m_values[ Jy ] * appendThis.m_values[ Iy ] + m_values[ Ky ] * appendThis.m_values[ Iz ] + m_values[ Ty ] * appendThis.m_values[ Iw ];
	result[ Iz ] = m_values[ Iz ] * appendThis.m_values[ Ix ] + m_values[ Jz ] * appendThis.m_values[ Iy ] + m_values[ Kz ] * appendThis.m_values[ Iz ] + m_values[ Tz ] * appendThis.m_values[ Iw ];
	result[ Iw ] = m_values[ Iw ] * appendThis.m_values[ Ix ] + m_values[ Jw ] * appendThis.m_values[ Iy ] + m_values[ Kw ] * appendThis.m_values[ Iz ] + m_values[ Tw ] * appendThis.m_values[ Iw ];

	result[ Jx ] = m_values[ Ix ] * appendThis.m_values[ Jx ] + m_values[ Jx ] * appendThis.m_values[ Jy ] + m_values[ Kx ] * appendThis.m_values[ Jz ] + m_values[ Tx ] * appendThis.m_values[ Jw ];
	result[ Jy ] = m_values[ Iy ] * appendThis.m_values[ Jx ] + m_values[ Jy ] * appendThis.m_values[ Jy ] + m_values[ Ky ] * appendThis.m_values[ Jz ] + m_values[ Ty ] * appendThis.m_values[ Jw ];
	result[ Jz ] = m_values[ Iz ] * appendThis.m_values[ Jx ] + m_values[ Jz ] * appendThis.m_values[ Jy ] + m_values[ Kz ] * appendThis.m_values[ Jz ] + m_values[ Tz ] * appendThis.m_values[ Jw ];
	result[ Jw ] = m_values[ Iw ] * appendThis.m_values[ Jx ] + m_values[ Jw ] * appendThis.m_values[ Jy ] + m_values[ Kw ] * appendThis.m_values[ Jz ] + m_values[ Tw ] * appendThis.m_values[ Jw ];

	result[ Kx ] = m_values[ Ix ] * appendThis.m_values[ Kx ] + m_values[ Jx ] * appendThis.m_values[ Ky ] + m_values[ Kx ] * appendThis.m_values[ Kz ] + m_values[ Tx ] * appendThis.m_values[ Kw ];
	result[ Ky ] = m_values[ Iy ] * appendThis.m_values[ Kx ] + m_values[ Jy ] * appendThis.m_values[ Ky ] + m_values[ Ky ] * appendThis.m_values[ Kz ] + m_values[ Ty ] * appendThis.m_values[ Kw ];
	result[ Kz ] = m_values[ Iz ] * appendThis.m_values[ Kx ] + m_values[ Jz ] * appendThis.m_values[ Ky ] + m_values[ Kz ] * appendThis.m_values[ Kz ] + m_values[ Tz ] * appendThis.m_values[ Kw ];
	result[ Kw ] = m_values[ Iw ] * appendThis.m_values[ Kx ] + m_values[ Jw ] * appendThis.m_values[ Ky ] + m_values[ Kw ] * appendThis.m_values[ Kz ] + m_values[ Tw ] * appendThis.m_values[ Kw ];

	result[ Tx ] = m_values[ Ix ] * appendThis.m_values[ Tx ] + m_values[ Jx ] * appendThis.m_values[ Ty ] + m_values[ Kx ] * appendThis.m_values[ Tz ] + m_values[ Tx ] * appendThis.m_values[ Tw ];
	result[ Ty ] = m_values[ Iy ] * appendThis.m_values[ Tx ] + m_values[ Jy ] * appendThis.m_values[ Ty ] + m_values[ Ky ] * appendThis.m_values[ Tz ] + m_values[ Ty ] * appendThis.m_values[ Tw ];
	result[ Tz ] = m_values[ Iz ] * appendThis.m_values[ Tx ] + m_values[ Jz ] * appendThis.m_values[ Ty ] + m_values[ Kz ] * appendThis.m_values[ Tz ] + m_values[ Tz ] * appendThis.m_values[ Tw ];
	result[ Tw ] = m_values[ Iw ] * appendThis.m_values[ Tx ] + m_values[ Jw ] * appendThis.m_values[ Ty ] + m_values[ Kw ] * appendThis.m_values[ Tz ] + m_values[ Tw ] * appendThis.m_values[ Tw ];

	for ( int index = 0; index < 16; index++ )
	{
		m_values[ index ] = result[ index ];
	}
}


//-----------------------------------------------------------------------------------------------
void Mat44::AppendZRotation( float degreesRotationAboutZ )
{
	Mat44 rotationAboutZ = Mat44::CreateZRotationDegrees( degreesRotationAboutZ );
	Append( rotationAboutZ );
}


//-----------------------------------------------------------------------------------------------
void Mat44::AppendYRotation( float degreesRotationAboutY )
{
	Mat44 rotationAboutY = Mat44::CreateYRotationDegrees( degreesRotationAboutY );
	Append( rotationAboutY );
}


//-----------------------------------------------------------------------------------------------
void Mat44::AppendXRotation( float degreesRotationAboutX )
{
	Mat44 rotationAboutX = Mat44::CreateXRotationDegrees( degreesRotationAboutX );
	Append( rotationAboutX );
}


//-----------------------------------------------------------------------------------------------
void Mat44::AppendTranslation2D( Vec2 const& translationXY )
{
	Mat44 translation2D = Mat44::CreateTranslation2D( translationXY );
	Append( translation2D );
}


//-----------------------------------------------------------------------------------------------
void Mat44::AppendTranslation3D( Vec3 const& translationXYZ )
{
	Mat44 translation3D = Mat44::CreateTranslation3D( translationXYZ );
	Append( translation3D );
}


//-----------------------------------------------------------------------------------------------
void Mat44::AppendScaleUniform2D( float uniformScaleXY )
{
	Mat44 uniformScale2D = Mat44::CreateUniformScale2D( uniformScaleXY );
	Append( uniformScale2D );
}


//-----------------------------------------------------------------------------------------------
void Mat44::AppendScaleUniform3D( float uniformScaleXYZ )
{
	Mat44 uniformScale3D = Mat44::CreateUniformScale3D( uniformScaleXYZ );
	Append( uniformScale3D );
}


//-----------------------------------------------------------------------------------------------
void Mat44::AppendScaleNonUniform2D( Vec2 const& nonUniformScaleXY )
{
	Mat44 nonUniformScale2D = Mat44::CreateNonUniformScale2D( nonUniformScaleXY );
	Append( nonUniformScale2D );
}


//-----------------------------------------------------------------------------------------------
void Mat44::AppendScaleNonUniform3D( Vec3 const& nonUniformScaleXYZ )
{
	Mat44 nonUniformScale3D = Mat44::CreateNonUniformScale3D( nonUniformScaleXYZ );
	Append( nonUniformScale3D );
}


//-----------------------------------------------------------------------------------------------
Mat44 const Mat44::CreateOrthoProjection( float left, float right, float bottom, float top, float zNear, float zFar )
{
	Mat44 orthoMatrix;

	if ( right == left )
	{
		orthoMatrix.m_values[ orthoMatrix.Ix ] = 0.0f;
		orthoMatrix.m_values[ orthoMatrix.Tx ] = 0.0f;
	}

	else
	{
		orthoMatrix.m_values[ orthoMatrix.Ix ] = 2.0f / ( right - left );
		orthoMatrix.m_values[ orthoMatrix.Tx ] = ( right + left ) / ( left - right );
	}

	if ( bottom == top )
	{
		orthoMatrix.m_values[ orthoMatrix.Jy ] = 0.0f;
		orthoMatrix.m_values[ orthoMatrix.Ty ] = 0.0f;
	}

	else
	{
		orthoMatrix.m_values[ orthoMatrix.Jy ] = 2.0f / ( top - bottom );
		orthoMatrix.m_values[ orthoMatrix.Ty ] = ( top + bottom ) / ( bottom - top );
	}

	if ( zNear == zFar )
	{
		orthoMatrix.m_values[ orthoMatrix.Kz ] = 0.0f;
		orthoMatrix.m_values[ orthoMatrix.Tz ] = 0.0f;
	}

	else
	{
		orthoMatrix.m_values[ orthoMatrix.Kz ] = 1.0f / ( zFar - zNear );
		orthoMatrix.m_values[ orthoMatrix.Tz ] = zNear / ( zNear - zFar );
	}


	return orthoMatrix;
}


//-----------------------------------------------------------------------------------------------
Mat44 const Mat44::CreatePerspectiveProjection( float fovYDegrees, float aspect, float zNear, float zFar )
{
	Mat44 perspectiveMatrix;

	float c = CosDegrees( fovYDegrees * 0.5f );
	float s = SinDegrees( fovYDegrees * 0.5f );
	float scaleY = c / s;
	float scaleX = scaleY / aspect;
	float scaleZ = zFar / ( zFar - zNear );
	float translateZ = ( zNear * zFar ) / ( zNear - zFar );

	perspectiveMatrix.m_values[ Ix ] = scaleX;
	perspectiveMatrix.m_values[ Jy ] = scaleY;
	perspectiveMatrix.m_values[ Kz ] = scaleZ;
	perspectiveMatrix.m_values[ Kw ] = 1.f;
	perspectiveMatrix.m_values[ Tz ] = translateZ;
	perspectiveMatrix.m_values[ Tw ] = 0.f;

	return perspectiveMatrix;
}


//-----------------------------------------------------------------------------------------------
void Mat44::Transpose()
{
	*( this ) = GetTranspose();
}


//-----------------------------------------------------------------------------------------------
void Mat44::Orthonormalize_XFwd_YLeft_ZUp()
{
	Vec3 iBasis( m_values[ Ix ], m_values[ Iy ], m_values[ Iz ] );

	iBasis = iBasis.GetNormalized();

	Vec3 kBasis;
	Vec3 jBasis = CrossProduct3D( Vec3( 0.0f, 0.0f, 1.0f ), iBasis ).GetNormalized();

	if ( jBasis.GetLengthSquared() == 0 )
	{
		kBasis = CrossProduct3D( iBasis, Vec3( 0.0f, 1.0f, 0.0f ) );
		jBasis = CrossProduct3D( kBasis, iBasis );
	}

	else
	{
		kBasis = CrossProduct3D( iBasis, jBasis );
	}

	m_values[ Ix ] = iBasis.x;
	m_values[ Iy ] = iBasis.y;
	m_values[ Iz ] = iBasis.z;

	m_values[ Jx ] = jBasis.x;
	m_values[ Jy ] = jBasis.y;
	m_values[ Jz ] = jBasis.z;

	m_values[ Kx ] = kBasis.x;
	m_values[ Ky ] = kBasis.y;
	m_values[ Kz ] = kBasis.z;
}

