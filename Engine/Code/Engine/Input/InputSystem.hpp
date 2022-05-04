#pragma once
#include "XboxController.hpp"
#include "KeyButtonState.hpp"
#include <vector>
#include "Engine/Math/IntVec2.hpp"


//-----------------------------------------------------------------------------------------------
extern unsigned char const KEYCODE_F1;
extern unsigned char const KEYCODE_F2;
extern unsigned char const KEYCODE_F3;
extern unsigned char const KEYCODE_F4;
extern unsigned char const KEYCODE_F5;
extern unsigned char const KEYCODE_F6;
extern unsigned char const KEYCODE_F7;
extern unsigned char const KEYCODE_F8;
extern unsigned char const KEYCODE_F9;
extern unsigned char const KEYCODE_F10;
extern unsigned char const KEYCODE_F11;
extern unsigned char const KEYCODE_UPARROW;
extern unsigned char const KEYCODE_DOWNARROW;
extern unsigned char const KEYCODE_INSERT;
extern unsigned char const KEYCODE_LEFTARROW;
extern unsigned char const KEYCODE_RIGHTARROW;
extern unsigned char const KEYCODE_SPACE;
extern unsigned char const KEYCODE_ESCAPE;
extern unsigned char const KEYCODE_ENTER;
extern unsigned char const KEYCODE_TILDE;
extern unsigned char const KEYCODE_BACKSPACE;
extern unsigned char const KEYCODE_DELETE;
extern unsigned char const KEYCODE_HOME;
extern unsigned char const KEYCODE_END;
extern unsigned char const KEYCODE_SHIFT;
extern unsigned char const KEYCODE_SQUARE_OPEN_BRACKET;
extern unsigned char const KEYCODE_SQUARE_CLOSE_BRACKET;
extern unsigned char const KEYCODE_COMMA;
extern unsigned char const KEYCODE_PERIOD;
extern unsigned char const KEYCODE_PLUS_AND_EQUALS;
extern unsigned char const KEYCODE_MINUS;



//-----------------------------------------------------------------------------------------------
constexpr int NUM_KEYCODES = 256;
constexpr int NUM_XBOX_CONTROLLERS = 4;
constexpr int NUM_MOUSE_BUTTONS = 3;


//------------------------------------------------------------------------------------------------
struct MouseConfig
{
	int m_priority = 0;
	bool m_isHidden = false;
	bool m_isLocked = false;
	bool m_isRelative = false;
};



//-----------------------------------------------------------------------------------------------
struct InputConfig
{

};


//-----------------------------------------------------------------------------------------------
class InputSystem
{
public:
	InputSystem( InputConfig const& config );
	~InputSystem();
	void Startup();
	void Shutdown();
	void BeginFrame();
	void EndFrame();

	bool WasKeyJustPressed( unsigned char keyCode );
	bool WasKeyJustReleased( unsigned char keyCode );
	bool IsKeyDown( unsigned char keyCode );
	bool HandleKeyPressed( unsigned char keyCode );
	bool HandleKeyReleased( unsigned char keyCode );

	bool WasMouseKeyJustPressed( unsigned char keyCode );
	bool WasMouseKeyJustReleased( unsigned char keyCode );
	bool IsMouseKeyDown( unsigned char keyCode );
	bool HandleMouseButtonPressed( unsigned char mouseKeyCode );
	bool HandleMouseButtonReleased( unsigned char mouseKeyCode );

	XboxController const& GetController( int controllerId );

	void ClearState();

	void SetDefaultMousePosition();
	void SetRelativeMousePosition( IntVec2 pos );

	void PushMouseConfig( MouseConfig const* config );              // add a config - referenced by its address (config must be valid for lifetime it is pushed)
	void PopMouseConfig( MouseConfig const* config );
	void ApplyTopMode();

	IntVec2 GetMouseClientPosition() const;
	IntVec2 GetMouseRelativeMovement() const;

protected:

	std::vector<MouseConfig const*> m_priorityStack;
	MouseConfig                     m_defaultConfig;
	MouseConfig                     m_currentConfig;

	IntVec2                         m_relativeMousePosition = IntVec2( 0, 0 );
	IntVec2                         m_previousClientPosition = IntVec2( 0, 0 );

	InputConfig	                    m_config;
	KeyButtonState                  m_keyStates[ NUM_KEYCODES ];
	XboxController                  m_controllers[ NUM_XBOX_CONTROLLERS ];

	KeyButtonState                  m_mouseButtons[ NUM_MOUSE_BUTTONS ];
};