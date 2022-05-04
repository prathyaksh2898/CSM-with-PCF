#pragma once
#include <string>
#include "Rgba8.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Math/AABB2.hpp"
#include <vector>
#include <mutex>
#include "Engine/Input/InputSystem.hpp"


//----------------------------------------------------------------------------------------------------
class BitmapFont;
class Renderer;
class InputSystem;
class Clock;
class Stopwatch;


//----------------------------------------------------------------------------------------------------
struct DevConsoleConfig
{
	InputSystem* m_defaultInputSystem  = nullptr;
	Renderer*    m_defaultRenderer     = nullptr;
	std::string  m_defaultFontName     = "System";
	float        m_defaultFontAspect   = 0.8f;
	float        m_linesOnScreen       = 30.5f;
};


//----------------------------------------------------------------------------------------------------
struct DevConsoleLine
{
	std::string text = "";
	Rgba8 color = Rgba8::BLUE;
};


//----------------------------------------------------------------------------------------------------
enum class DevConsoleMode
{
	VISIBLE,
	HIDDEN,
	NUM_MODES
};


//----------------------------------------------------------------------------------------------------
class DevConsole
{
public:
	DevConsole(DevConsoleConfig const& config);
	~DevConsole();

	void Startup();
	void BeginFrame();
	void EndFrame();
	void Shutdown();

	static bool HandleCharInputFromWindows( EventArgs& args );
	static bool HandleKeyDownInputFromWindows( EventArgs& args );
	static bool ClearDevConsole( EventArgs& args );

	void Update();
	void Execute( std::string const& consoleCommandText );
	void AddLine( Rgba8 const& color, std::string const& text );
	void Render( AABB2 const& bounds, Renderer* rendererOverrride = nullptr )const;

	DevConsoleMode GetMode() const;
	void SetMode( DevConsoleMode mode );
	void ToggleMode( DevConsoleMode mode );

	bool IsProcessingInput();

	static Rgba8 const ERROR;
	static Rgba8 const SUCCESS;
	static Rgba8 const WARNING;
	static Rgba8 const INFO_MAJOR;
	static Rgba8 const INFO_MINOR;

protected:
	void Render_OpenFull( AABB2 const& bounds, Renderer& renderer, BitmapFont& font, float fontAspect = 1.f ) const;

protected:
	DevConsoleConfig             m_config;
	DevConsoleMode               m_mode = DevConsoleMode::HIDDEN;
	
	Clock*                       m_consoleClock = nullptr;
	Stopwatch*                   m_caretStopwatch = nullptr;

	MouseConfig                  m_mouseConfig;

	std::mutex                   m_devConsoleLock;

	std::vector<DevConsoleLine>  m_lines;
	std::vector<std::string>     m_commandHistory;
	std::string                  m_activeCommand;

	int                          m_commandHistoryLocation = 0;
	int                          m_caretLocation = 0;
	bool                         m_caretVisible = true;
	bool                         m_handlingInput = false;
	int                          m_frameNumber;
};