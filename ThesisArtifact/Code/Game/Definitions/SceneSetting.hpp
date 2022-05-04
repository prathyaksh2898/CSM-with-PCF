#pragma once
#include "Game/GameCommon.hpp"

#include "Engine/Core/XMLUtils.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/EulerAngles.hpp"

#include <string>
#include <vector>


//------------------------------------------------------------------------------------------------
class LightConfiguration;
class FBXSceneObject;


//------------------------------------------------------------------------------------------------
class SceneSetting
{
public:
	bool                       LoadFromXmlElement( XmlElement const& elem );
	static SceneSetting const* GetSceneSettingByID( int id );
	static SceneSetting const* GetSceneSettingByName( std::string sceneName );

public:
	int                               m_id;
	std::string                       m_name;
	LightConfiguration const*         m_lightConfig = nullptr;
	std::vector<FBXSceneObject*>   	  m_sceneObjects;

	Vec3                              m_cam1Position;
	EulerAngles                       m_cam1Orientation;
	Vec3                              m_cam2Position;
	EulerAngles                       m_cam2Orientation;
	bool                              m_useCamera1 = true;

	bool                              m_debugSpecificLight = false;
	bool                              m_debugFrustum = false;
	bool                              m_debugCascades = false;
	bool                              m_debugSpecCascades = false;
	bool                              m_debugDepthBuffer = false;
	bool                              m_debugAllDepthBuffers = false;
	int                               m_debugCascadeNum = 0;
	int                               m_specCascadeNum = 0;

	float                             m_nearPlane = 0.001f;
	float                             m_farPlane  = 200.0f;

	int                               m_numSamples = 16;

	bool                              m_enablePCF = true;
	bool                              m_hideDefaultGeometry = false;

	static std::vector<SceneSetting*> s_sceneDefs;
};

