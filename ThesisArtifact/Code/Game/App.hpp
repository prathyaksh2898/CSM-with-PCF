#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Game/EngineBuildPreferences.hpp"


//----------------------------------------------------------------------------------------------------
class Game;
struct MouseConfig;


//----------------------------------------------------------------------------------------------------
class App
{
public:
	App();
	~App();
	void Startup();
	void Shutdown();
	void Run();
	void RunFrame();

	bool HandleQuitRequested();
	static bool QuitApp( EventArgs& args );

private:
	void BeginFrame();
	void Update( float deltaSeconds );
	void Render() const;
	void EndFrame();
	void ResetGame();
	void UpdateDevKeys();


private:
	bool         m_isQuitting = false;
	Game*        m_theGame    = nullptr;

	MouseConfig* m_defaultConfig;
};