#include "LightConfigurations.hpp"

#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/Vec3.hpp"


//------------------------------------------------------------------------------------------------
std::vector<LightConfiguration*> LightConfiguration::s_lightDefs;


//------------------------------------------------------------------------------------------------
bool LightConfiguration::LoadFromXmlElement( XmlElement const& elem )
{
	GUARANTEE_OR_DIE( _strcmpi( elem.Name(), "LightConfiguration" ) == 0, "Error In XML Format" );

	tinyxml2::XMLElement const* element = &elem;
	m_id = ParseXmlAttribute( *element, "id", 9999 );

	ASSERT_OR_DIE( GetLightConfigurationByID( m_id ) == nullptr, "Duplicate Data Found In LightConfiguration" );

	LightDataC data;
	
	data.m_worldPosition       = ParseXmlAttribute( *element, "position", data.m_worldPosition );
	data.m_intensity           = ParseXmlAttribute( *element, "intensity", data.m_intensity );
	
	Vec3 orientation           = ParseXmlAttribute( *element, "orientation", Vec3::ZERO );
	Mat44 orientMat;
	orientMat.AppendZRotation( orientation.x );
	orientMat.AppendYRotation( orientation.y );
	orientMat.AppendXRotation( orientation.z );
	Vec3 dir = Vec3( 1.0f, 0.0f, 0.0f );
	dir                        = orientMat.TransformVectorQuantity3D( dir );
	dir.Normalize();
	data.m_direction           = dir;
	
	data.m_direction           = ParseXmlAttribute( *element, "direction", data.m_direction );
	data.m_attenuation         = ParseXmlAttribute( *element, "attenuation", data.m_attenuation );
	data.m_specularAttenuation = ParseXmlAttribute( *element, "specularAttenuation", data.m_specularAttenuation );
	data.m_dotInnerAngle       = ParseXmlAttribute( *element, "dotInnerAngle", data.m_dotInnerAngle );
	data.m_dotOuterAngle       = ParseXmlAttribute( *element, "dotOuterAngle", data.m_dotOuterAngle );
	data.m_ShadowBias          = ParseXmlAttribute( *element, "shadowBias",  data.m_ShadowBias );
	data.m_isShadowCasting     = ( ParseXmlAttribute( *element, "shadowCasting", 0 ) );

	Rgba8 color                = ParseXmlAttribute( *element, "color", Rgba8( 0, 0, 0, 0 ) );
	float colorAsFloats[ 4 ];
	color.GetAsFloats( colorAsFloats );
	data.m_color.x = colorAsFloats[0];
	data.m_color.y = colorAsFloats[1];
	data.m_color.z = colorAsFloats[2];

	//Cascade values
	m_numCascades = ParseXmlAttribute( *element, "numCascades", 1 );
	
	for ( uint cascadeNum = 0; cascadeNum < m_numCascades && cascadeNum < NUM_CASCADES; cascadeNum++ )
	{
		m_cascadePercentages[cascadeNum] = ParseXmlAttribute( *element, Stringf( "cascade%d", cascadeNum ).c_str(), 10 );
		if ( cascadeNum != 0 && m_cascadePercentages[cascadeNum] < m_cascadePercentages[cascadeNum - 1] )
		{
			for ( int recursiveCascade = cascadeNum - 1; recursiveCascade >= 0; recursiveCascade-- )
			{
				if ( m_cascadePercentages[cascadeNum] < m_cascadePercentages[recursiveCascade] )
				{
					m_cascadePercentages[recursiveCascade] = m_cascadePercentages[cascadeNum];
				}
			}
		}

		if ( cascadeNum != NUM_CASCADES - 1 && m_cascadePercentages[cascadeNum] > m_cascadePercentages[cascadeNum + 1] )
		{
			for ( int recursiveCascade = cascadeNum + 1; recursiveCascade < NUM_CASCADES; recursiveCascade++ )
			{
				if ( m_cascadePercentages[cascadeNum] > m_cascadePercentages[recursiveCascade] )
				{
					m_cascadePercentages[recursiveCascade] = m_cascadePercentages[cascadeNum];
				}
			}
		}
	}
	
	bool isLightRotating = ParseXmlAttribute( *element, "isRotating", 0 );
	int  rotatingAxis    = ParseXmlAttribute( *element, "rotatingAxis", 0 );

	tinyxml2::XMLElement const* childOfElement = element->FirstChildElement();
	int index = 0;

	while ( childOfElement && index < MAXLIGHTS )
	{
		std::string lightType = ParseXmlAttribute( *childOfElement, "lightType", "" );

		if ( _strcmpi( "PointLight", lightType.c_str() ) == 0 )
		{
			m_shaderData.m_lights[ index ].m_lightType = POINT_LIGHT;
			m_shaderData.m_lights[ index ].m_directionFactor = 0.0f;
		}
		else if ( _strcmpi( "Spotlight", lightType.c_str() ) == 0 )
		{
			m_shaderData.m_lights[ index ].m_lightType = SPOT_LIGHT;
			m_shaderData.m_lights[ index ].m_directionFactor = 1.0f;
		}
		else
		{
			m_shaderData.m_lights[ index ].m_lightType = DIRECTIONAL_LIGHT;
			m_shaderData.m_lights[ index ].m_directionFactor = 1.0f;
		}

		m_shaderData.m_lights[index].m_worldPosition        = ParseXmlAttribute( *childOfElement, "position", Vec3( 777.0f, 777.0f, 777.0f ) );
		if ( m_shaderData.m_lights[ index ].m_worldPosition == Vec3( 777.0f, 777.0f, 777.0f ) )
		{
			m_shaderData.m_lights[ index ].m_worldPosition = data.m_worldPosition;
		}
		
		m_shaderData.m_lights[index].m_intensity           = ParseXmlAttribute( *childOfElement, "intensity", -1.0f );
		if ( m_shaderData.m_lights[ index ].m_intensity == -1.0f )
		{
			m_shaderData.m_lights[ index ].m_intensity = data.m_intensity;
		}

		Vec3 lightOrientation = ParseXmlAttribute( *childOfElement, "orientation", Vec3( 777.0f, 777.0f, 777.0f ) );
		if ( !( lightOrientation == Vec3( 777.0f, 777.0f, 777.0f ) ) )
		{
			Mat44 lightOrientMat;
			lightOrientMat.AppendZRotation( lightOrientation.x );
			lightOrientMat.AppendYRotation( lightOrientation.y );
			lightOrientMat.AppendXRotation( lightOrientation.z );
			Vec3 lightOrientDir = Vec3( 1.0f, 0.0f, 0.0f );
			lightOrientDir                             = lightOrientMat.TransformVectorQuantity3D( lightOrientDir );
			lightOrientDir.Normalize();
			m_shaderData.m_lights[ index ].m_direction = lightOrientDir;
		}

		m_shaderData.m_lights[index].m_direction        = ParseXmlAttribute( *childOfElement, "direction", m_shaderData.m_lights[ index ].m_direction );
		if ( m_shaderData.m_lights[ index ].m_direction == Vec3::ZERO )
		{
			m_shaderData.m_lights[ index ].m_direction = data.m_direction;
		}
		
		Rgba8 eleColor = ParseXmlAttribute( *childOfElement, "color", Rgba8(0, 0, 0, 0) );
		if ( eleColor == Rgba8( 0, 0, 0, 0 ) )
		{
			m_shaderData.m_lights[ index ].m_color = data.m_color;
		}
		else
		{
			float floatColors[ 4 ];
			eleColor.GetAsFloats( floatColors );
			m_shaderData.m_lights[ index ].m_color.x = floatColors[ 0 ];
			m_shaderData.m_lights[ index ].m_color.y = floatColors[ 1 ];
			m_shaderData.m_lights[ index ].m_color.z = floatColors[ 2 ];
		}
		
		m_shaderData.m_lights[index].m_attenuation         = ParseXmlAttribute( *childOfElement, "attenuation", Vec3( 777.0f, 777.0f, 777.0f ) );
		if ( m_shaderData.m_lights[ index ].m_attenuation == Vec3( 777.0f, 777.0f, 777.0f ) )
		{
			m_shaderData.m_lights[ index ].m_attenuation = data.m_attenuation;
		}
		
		m_shaderData.m_lights[ index ].m_specularAttenuation = ParseXmlAttribute( *childOfElement, "specularAttenuation", Vec3( 777.0f, 777.0f, 777.0f ) );
		if ( m_shaderData.m_lights[ index ].m_specularAttenuation == Vec3( 777.0f, 777.0f, 777.0f ) )
		{
			m_shaderData.m_lights[ index ].m_specularAttenuation = data.m_specularAttenuation;
		}
		
		m_shaderData.m_lights[ index ].m_dotInnerAngle = ParseXmlAttribute( *childOfElement, "dotInnerAngle", -1.0f );
		if ( m_shaderData.m_lights[ index ].m_dotInnerAngle == -1.0f )
		{
			m_shaderData.m_lights[ index ].m_dotInnerAngle = data.m_dotInnerAngle;
		}
		
		m_shaderData.m_lights[ index ].m_dotOuterAngle = ParseXmlAttribute( *childOfElement, "dotOuterAngle", -1.0f );
		if ( m_shaderData.m_lights[ index ].m_dotOuterAngle == -1.0f )
		{
			m_shaderData.m_lights[ index ].m_dotOuterAngle = data.m_dotOuterAngle;
		}
		
		m_shaderData.m_lights[ index ].m_ShadowBias    = ParseXmlAttribute( *childOfElement, "shadowBias", -1.0f );
		if ( m_shaderData.m_lights[ index ].m_ShadowBias == -1.0f )
		{
			m_shaderData.m_lights[ index ].m_ShadowBias = data.m_ShadowBias;
		}

		m_shaderData.m_lights[index].m_isShadowCasting = ParseXmlAttribute( *childOfElement, "shadowCasting", 3 );
		if ( m_shaderData.m_lights[index].m_isShadowCasting == 3 )
		{
			m_shaderData.m_lights[index].m_isShadowCasting = data.m_isShadowCasting;
		}

		m_lightRotating[index] = ParseXmlAttribute( *childOfElement, "isRotating", -1 );
		
		if ( m_lightRotating[index] == -1 )
			m_lightRotating[index] = isLightRotating;
		
		m_lightRotatingAxis[index] = ParseXmlAttribute( *childOfElement, "rotatingAxis", -1 );
		if ( m_lightRotatingAxis[index] == -1 )
			m_lightRotatingAxis[index] = rotatingAxis;


		if ( m_shaderData.m_lights[index].m_lightType != DIRECTIONAL_LIGHT )
		{
			m_lightMoving[index] = ParseXmlAttribute( *childOfElement, "isMoving", false );
			m_lightMoveSpeed[index] = ParseXmlAttribute( *childOfElement, "moveSpeed", 0.0f );
			m_lightStartPosition[index] = ParseXmlAttribute( *childOfElement, "startPosition", Vec3( 777.0f, 777.0f, 777.0f ) );
			m_lightEndPosition[index] = ParseXmlAttribute( *childOfElement, "endPosition", Vec3( 777.0f, 777.0f, 777.0f ) );
		}

		index++;
		childOfElement = childOfElement->NextSiblingElement();
	}

	m_numLights = index;

	s_lightDefs.push_back( this );

	return true;
}


//------------------------------------------------------------------------------------------------
LightConfiguration const* LightConfiguration::GetLightConfigurationByID( int id )
{
	for ( LightConfiguration const* lightConfig : s_lightDefs )
	{
		if ( id == lightConfig->m_id )
		{
			return lightConfig;
		}
	}

	return nullptr;
}

