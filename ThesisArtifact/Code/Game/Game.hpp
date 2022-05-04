#pragma once
#include "GameCommon.hpp"

#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/LightStructure.hpp"
#include "Engine/Renderer/VertexData/Vertex_PCU.hpp"
#include "Engine/Renderer/VertexData/Vertex_PCUTBN.hpp"


//----------------------------------------------------------------------------------------------------
class App;
class BitmapFont;
class Clock;
class ConstantBuffer;
class Object;
class Prop;
class Player;
class SceneSetting;
class Stopwatch;
class Texture;
class VertexBuffer;

struct MouseConfig;
struct LightCamera;
struct ShaderLightData;


//----------------------------------------------------------------------------------------------------
struct CameraConstantsForCamera1
{
	Mat44 m_viewMatrix;
	Mat44 m_projectionMatrix;
};


//----------------------------------------------------------------------------------------------------
struct DebugCascadePrint
{
	float debugCascadeLayer = 0.0f;
	Vec3  padding;
};


//----------------------------------------------------------------------------------------------------
class Game
{

public:
	Game( App* owner );
	~Game();

	void Startup();
		void CreateMouseConfigs();
		void CreateOrigin();
		void CreateEntities();
		void CreateLightConfigs();

	void Update();
		void UpdateDebug();
		void UpdateShaderLightDataUsingUI();
		void UpdateLightRotation( float deltaSeconds );
		void UpdateEntities( float deltaSeconds );
		void UpdateCamera( float deltaSeconds );
		     void UpdateLightCameraProjection( int lightNum );
		void AddVertsRendered( uint32_t vertsAdded );

	void Render() const;
		void RenderForDepthBuffers() const;
		void RenderEntities() const;
		void RenderUI() const;
	
	void Shutdown();

	//Utilities
	void ResetEntities();
	void ReloadXMLData();
	void LoadNextScene();
	void LoadPreviousScene();
	    void LoadScene( uint sceneNum );
	
private:
	App*                       m_App;


//--------------------------------------------------------------------------------------------------------------------------------------------
//			MOUSE CONFIGs
//--------------------------------------------------------------------------------------------------------------------------------------------
							   					   			       
	MouseConfig                m_playingConfig;
	MouseConfig                m_ImGuiConfig;

						
//--------------------------------------------------------------------------------------------------------------------------------------------
//			CAMERAS
//--------------------------------------------------------------------------------------------------------------------------------------------

	Camera                     m_worldCamera;
	Camera                     m_worldCamera2;
	Camera                     m_screenCamera;

	bool                       m_useCamera1     = true;
	bool                       m_isCamera1Ortho = false;
	
//--------------------------------------------------------------------------------------------------------------------------------------------
//			LIGHTS
//--------------------------------------------------------------------------------------------------------------------------------------------
	
	ShaderLightData            m_shaderLightData;
	LightDataC                 m_lightPresets[ 4 ];
	
	LightCamera**              m_lightCameraArray;

	ConstantBuffer*            m_cascadeDepthConstantBuffer = nullptr;
	ConstantBuffer*            m_cam1ConstantBuffer         = nullptr;
	ConstantBuffer*            m_debugPrintConstantBuffer   = nullptr;

	CascadeConstantsData       m_cascadeData;
	bool                       m_enablePCF;
									   

//--------------------------------------------------------------------------------------------------------------------------------------------
//			MODEL DATA
//--------------------------------------------------------------------------------------------------------------------------------------------	
		
	std::vector<Vertex_PCUTBN> m_floor;
	std::vector<Vertex_PCUTBN> m_wall;
	VertexBuffer*              m_cubeBuffer                 = nullptr;
	std::vector<Vertex_PCUTBN> m_cubeVerts1;
	Mat44                      m_cube1transform;
	Mat44                      m_cubeTransforms[ 1 ];
	bool                       m_hideDefaultGeometry        = false;

	Texture*                   m_skybox = nullptr;
	std::vector<Vertex_PCU>    m_skyBoxVerts;


//--------------------------------------------------------------------------------------------------------------------------------------------
//		    IMGUI DATA
//--------------------------------------------------------------------------------------------------------------------------------------------	

	bool                       m_ImGui                = false;
	bool                       m_debugSpecificLight   = false;
	bool                       m_debugFrustum         = false;
	bool                       m_debugCascades        = false;
	bool                       m_debugSpecCascades    = false;
	bool                       m_debugDepthBuffer     = false;
	bool                       m_debugAllDepthBuffers = false;
	int                        m_debugCascadeNum      = 0;
	int                        m_specCascadeNum       = 0;
	int                        m_numCascades          = 1;
													  
	int                        m_numLights            = 0;
	bool                       m_lightCamFollow[ MAXLIGHTS ];
	bool                       m_lightCamDir[ MAXLIGHTS ];

	int                        m_debugLightNumber   = 0;

	float                      m_nearPlane          = 0.001f;
	float                      m_farPlane           = 200.0f;
	int                        m_cascadeDepthPercent[NUM_CASCADES];

	bool                       m_lightRotating[MAXLIGHTS];
	uint                       m_lightRotationAxes[MAXLIGHTS];
	Mat44                      m_rotationMatrixPlusX;
	Mat44                      m_rotationMatrixPlusY;
	Mat44                      m_rotationMatrixMinusX;
	Mat44                      m_rotationMatrixMinusY;

	bool                       m_lightMoving[MAXLIGHTS];
	float                      m_lightMoveSpeed[MAXLIGHTS];
	Vec3                       m_lightStartPosition[MAXLIGHTS];
	Vec3                       m_lightEndPosition[MAXLIGHTS];
	
	int                        m_activeGameScene    = 0;
	SceneSetting*              m_sceneSetting       = nullptr;

	uint32_t                   m_vertsRendered = 0;

	Player*                    m_player;
};
