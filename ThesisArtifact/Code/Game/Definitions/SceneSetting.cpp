#include "SceneSetting.hpp"

#include "Game/Definitions/LightConfigurations.hpp"
#include "Game/FBXSceneObject.hpp"

#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Renderer/Renderer.hpp"

//------------------------------------------------------------------------------------------------
std::vector<SceneSetting*> SceneSetting::s_sceneDefs;


//------------------------------------------------------------------------------------------------
bool SceneSetting::LoadFromXmlElement( XmlElement const& elem )
{
	GUARANTEE_OR_DIE( _strcmpi( elem.Name(), "Scene" ) == 0, "Error In XML Format" );

	tinyxml2::XMLElement const* element = &elem;
	m_id = ParseXmlAttribute( *element, "id", 9999 );

	ASSERT_OR_DIE( GetSceneSettingByID( m_id ) == nullptr, "Duplicate Data Found In Scene Settings" );

	uint lightConfig = ParseXmlAttribute( *element, "lightConfig", 9999 );
	LightConfiguration const* config = LightConfiguration::GetLightConfigurationByID( lightConfig );
	m_lightConfig = config;
	ASSERT_OR_DIE( config != nullptr, "LightConfig not found" );

	m_name = ParseXmlAttribute( *element, "name", "" );

	tinyxml2::XMLElement const* firstFBXChild = elem.FirstChildElement("FBXModel");

	m_cam1Position = ParseXmlAttribute( *element, "camera1Position", Vec3::ZERO );
	Vec3 camOrientation = ParseXmlAttribute( *element, "camera1Orientation", Vec3() );
	m_cam1Orientation.m_yawDegrees   = camOrientation.x;
	m_cam1Orientation.m_pitchDegrees = camOrientation.y;
	m_cam1Orientation.m_rollDegrees  = camOrientation.z;

	m_cam2Position = ParseXmlAttribute( *element, "camera2Position", Vec3::ZERO );
	camOrientation = ParseXmlAttribute( *element, "camera2Orientation", Vec3() );
	m_cam2Orientation.m_yawDegrees   = camOrientation.x;
	m_cam2Orientation.m_pitchDegrees = camOrientation.y;
	m_cam2Orientation.m_rollDegrees  = camOrientation.z;

	m_useCamera1 = ParseXmlAttribute( *element, "useCamera1", true );

	m_debugSpecificLight   = ParseXmlAttribute( *element, "debugSpecificLight", false );
	m_debugFrustum         = ParseXmlAttribute( *element, "debugFrustum", false );
	m_debugCascades        = ParseXmlAttribute( *element, "debugCascades", false );
	m_debugSpecCascades    = ParseXmlAttribute( *element, "debugSpecCascades", false );
	m_debugDepthBuffer     = ParseXmlAttribute( *element, "debugDepthBuffer", false );
	m_debugAllDepthBuffers = ParseXmlAttribute( *element, "debugAllDepthBuffers", false );
	m_debugCascadeNum      = ParseXmlAttribute( *element, "debugCascadeNum", 0 );
	m_specCascadeNum       = ParseXmlAttribute( *element, "specCascadeNum", 0 );
	
	m_numSamples           = ParseXmlAttribute( *element, "numSamples", 16 );
	m_nearPlane            = ParseXmlAttribute( *element, "nearPlane", 0.001f );
	m_farPlane             = ParseXmlAttribute( *element, "farPlane", 200.0f);

	m_enablePCF = ParseXmlAttribute( *element, "enablePCF", true );

	m_hideDefaultGeometry = ParseXmlAttribute( *element, "hideDefaultGeometry", false );

	while ( firstFBXChild )
	{
		Vec3 position    = ParseXmlAttribute( *firstFBXChild, "position", Vec3(777.0f, 777.0f, 777.0f) );
		EulerAngles fbxOrientation;
		Vec3 orientation = ParseXmlAttribute( *firstFBXChild, "orientation", Vec3::ZERO );
		fbxOrientation.m_yawDegrees   = orientation.x;
		fbxOrientation.m_pitchDegrees = orientation.y;
		fbxOrientation.m_rollDegrees  = orientation.z;

		std::string fbxPath           = ParseXmlAttribute( *firstFBXChild, "path", "" );
		std::string texturePath       = ParseXmlAttribute( *firstFBXChild, "texturePath", "" );
		std::string normalTexturePath = ParseXmlAttribute( *firstFBXChild, "normalTexturePath", "" );

		FBXSceneObject* object = new FBXSceneObject( g_theGame, position, fbxOrientation, fbxPath );
		m_sceneObjects.push_back( object );

		if ( texturePath.length() > 1 )
		{
			object->m_texture       = g_theRenderer->CreateOrGetTextureFromFile( texturePath.c_str() );
		}
		if ( normalTexturePath.length() > 1 )
		{
			object->m_normalTexture = g_theRenderer->CreateOrGetTextureFromFile( texturePath.c_str() );
		}		
		
		firstFBXChild = firstFBXChild->NextSiblingElement("FBXModel");
	}

	s_sceneDefs.push_back( this );

	return true;
}


//------------------------------------------------------------------------------------------------
SceneSetting const* SceneSetting::GetSceneSettingByID( int id )
{
	for ( SceneSetting const* sceneDef : s_sceneDefs )
	{
		if ( id == sceneDef->m_id )
		{
			return sceneDef;
		}
	}

	return nullptr;
}


//------------------------------------------------------------------------------------------------
SceneSetting const* SceneSetting::GetSceneSettingByName( std::string sceneName )
{
	for ( SceneSetting const* sceneDef : s_sceneDefs )
	{
		if ( _strcmpi( sceneName.c_str(), sceneDef->m_name.c_str() ) == 0 )
		{
			return sceneDef;
		}
	}

	return nullptr;
}
