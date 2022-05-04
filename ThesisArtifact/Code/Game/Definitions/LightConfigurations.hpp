#pragma once
#include "Game/GameCommon.hpp"

#include "Engine/Core/XMLUtils.hpp"
#include "Engine/Renderer/LightStructure.hpp"

#include <string>
#include <vector>


//------------------------------------------------------------------------------------------------
class LightConfiguration
{
public:
	bool                             LoadFromXmlElement( XmlElement const& elem );
	static LightConfiguration const* GetLightConfigurationByID( int id );

public:
	int             m_id;
	uint            m_numLights;
	ShaderLightData m_shaderData;
	uint            m_numCascades;
	uint            m_cascadePercentages[NUM_CASCADES];
	
	int             m_lightRotating[MAXLIGHTS];
	int             m_lightRotatingAxis[MAXLIGHTS];

	bool            m_lightMoving[MAXLIGHTS];
	float           m_lightMoveSpeed[MAXLIGHTS];
	Vec3            m_lightStartPosition[MAXLIGHTS];
	Vec3            m_lightEndPosition[MAXLIGHTS];

	static std::vector<LightConfiguration*> s_lightDefs;
};

