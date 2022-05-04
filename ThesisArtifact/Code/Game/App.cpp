#include "Game.hpp"

#include "Game/GameCommon.hpp"

#include "Engine/3D/FBXLoader.hpp"
#include "Engine/3D/VisualDatabase.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Debug/UI/DebugUISystem.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Window/Window.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/VertexData/Vertex_PCU.hpp"
#include "Engine/Telemetry/D3D11PerformanceMarker.hpp"

#include <string>
#include <thread>

//----------------------------------------------------------------------------------------------- 
App::App()
{

}


//----------------------------------------------------------------------------------------------- 
App::~App()
{

}


//----------------------------------------------------------------------------------------------- 
void App::Startup()
{
	EventSystemConfig eventConfig;
	g_theEventSystem = new EventSystem( eventConfig );
	g_theEventSystem->Startup();

	InputConfig inputConfig;
	g_theInput = new InputSystem( inputConfig );
	g_theInput->Startup();

	WindowConfig windowConfig;
	windowConfig.m_inputSystem = g_theInput;
	windowConfig.m_aspectRatio = 2.f;
	windowConfig.m_windowTitle = "Cascaded Shadow Mapping - DirextX 11";
	g_theWindow = new Window( windowConfig );
	g_theWindow->Startup();

	RenderConfig renderConfig;
	renderConfig.m_window = g_theWindow;
	g_theRenderer = new Renderer( renderConfig );
	g_theRenderer->Startup();

	g_theD3D11PerfMarker = new D3D11PerformanceMarker( g_theRenderer );

	AudioConfig audioConfig;
	g_theAudio = new AudioSystem( audioConfig );
	g_theAudio->Startup();

	DevConsoleConfig devConfig;
	devConfig.m_defaultInputSystem = g_theInput;
	devConfig.m_defaultRenderer = g_theRenderer;
	devConfig.m_defaultFontAspect = 0.8f;
	devConfig.m_defaultFontName = "FixedFont";
	devConfig.m_linesOnScreen = 30.0f;
	g_theConsole = new DevConsole( devConfig );
	g_theConsole->Startup();

	DebugUIConfig debugConfig;
	debugConfig.m_renderer = g_theRenderer;
	debugConfig.m_window = g_theWindow;
	g_theDebugUISystem = new DebugUISystem( debugConfig );
	g_theDebugUISystem->Startup();

	FBXConfig fbxConfig;
	g_theFBXLoader = new FBXLoader( fbxConfig );
	g_theFBXLoader->Startup();

	VisualDatabaseConfig config;
	g_theVisualDatabase = new VisualDatabase( config );
	g_theVisualDatabase->Startup();

	m_theGame = new Game( this );
	m_theGame->Startup();

	SubscribeEventCallbackFunction( "Quit", App::QuitApp );
}


//----------------------------------------------------------------------------------------------- 
void App::BeginFrame()
{
	Clock::SystemBeginFrame();

	g_theEventSystem->BeginFrame();
	g_theInput->BeginFrame();
	g_theWindow->BeginFrame();
	g_theRenderer->BeginFrame();
	g_theAudio->BeginFrame();
	g_theConsole->BeginFrame();
	g_theDebugUISystem->BeginFrame();
}


//----------------------------------------------------------------------------------------------- 
void App::ResetGame()
{
	m_theGame->Shutdown();
	delete m_theGame;
	m_theGame = nullptr;

	m_theGame = new Game( this );
	m_theGame->Startup();
}


void App::Run()
{
	while ( !m_isQuitting )
	{
		RunFrame();
	}
}


//----------------------------------------------------------------------------------------------- 
void App::RunFrame()
{
	constexpr float MIN_FRAMES_PER_SECOND = 10.f;
	constexpr float MAX_DELTA_SECONDS = 1.f / MIN_FRAMES_PER_SECOND;
	float deltaSeconds = static_cast< float >( Clock::GetSystemClock().GetFrameDeltaSeconds() );

	if ( deltaSeconds > MAX_DELTA_SECONDS )
		deltaSeconds = MAX_DELTA_SECONDS;

	BeginFrame();
	Update( deltaSeconds );
	Render();
	EndFrame();

}


//----------------------------------------------------------------------------------------------- 
void App::Update( float deltaSeconds )
{
	g_theConsole->Update();
	g_theDebugUISystem->Update();
	UpdateDevKeys();
	UNUSED( deltaSeconds );
	m_theGame->Update();

}

//----------------------------------------------------------------------------------------------- 
void App::UpdateDevKeys()
{
	if ( g_theInput->WasKeyJustPressed( KEYCODE_F8 ) )
	{
		ResetGame();
	}

	if ( g_theInput->WasKeyJustPressed( KEYCODE_TILDE ) )
	{
		g_theConsole->SetMode( DevConsoleMode::VISIBLE );
	}

}


//----------------------------------------------------------------------------------------------- 
bool App::HandleQuitRequested()
{
	m_isQuitting = true;
	return true;
}


//-----------------------------------------------------------------------------------------------
bool App::QuitApp( EventArgs& args )
{
	g_theApp->HandleQuitRequested();
	UNUSED( args );
	return false;
}


//----------------------------------------------------------------------------------------------- 
void App::Render() const
{
	m_theGame->Render();
	g_theDebugUISystem->Render();
}


//----------------------------------------------------------------------------------------------- 
void App::EndFrame()
{
	if ( g_theInput->WasKeyJustPressed( KEYCODE_F1 ) )
	{
		g_theRenderer->RecompileAllShaders();
	}

	if ( g_theInput->WasKeyJustPressed( KEYCODE_F2 ) )
	{
		m_theGame->ReloadXMLData();
	}

	if ( g_theInput->WasKeyJustPressed( KEYCODE_SQUARE_OPEN_BRACKET ) )
	{
		m_theGame->LoadPreviousScene();
	}

	if ( g_theInput->WasKeyJustPressed( KEYCODE_SQUARE_CLOSE_BRACKET ) )
	{
		m_theGame->LoadNextScene();
	}

	g_theDebugUISystem->EndFrame();
	g_theConsole->EndFrame();
	g_theAudio->EndFrame();
	g_theRenderer->EndFrame();
	g_theWindow->EndFrame();
	g_theInput->EndFrame();
	g_theEventSystem->EndFrame();
}


//----------------------------------------------------------------------------------------------- 
void App::Shutdown()
{
	g_theInput->PopMouseConfig( m_defaultConfig );

	m_theGame->Shutdown();
	delete m_theGame;
	m_theGame = nullptr;

	g_theVisualDatabase->Shutdown();
	delete g_theVisualDatabase;
	g_theVisualDatabase = nullptr;

	g_theFBXLoader->Shutdown();
	delete g_theFBXLoader;
	g_theFBXLoader = nullptr;

	g_theDebugUISystem->Shutdown();
	delete g_theDebugUISystem;
	g_theDebugUISystem = nullptr;

	g_theConsole->Shutdown();
	delete g_theConsole;
	g_theConsole = nullptr;
	
	g_theAudio->Shutdown();
	delete g_theAudio;
	g_theAudio = nullptr;
	
	g_theRenderer->Shutdown();
	delete g_theRenderer;
	g_theRenderer = nullptr;
	
	g_theWindow->Shutdown();
	delete g_theWindow;
	g_theWindow = nullptr;
	
	g_theInput->Shutdown();
	delete g_theInput;
	g_theInput = nullptr;
	
	g_theEventSystem->Shutdown();
	delete g_theEventSystem;
	g_theEventSystem = nullptr;

}

