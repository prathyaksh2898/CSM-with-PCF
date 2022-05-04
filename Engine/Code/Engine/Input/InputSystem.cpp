#define WIN32_LEAN_AND_MEAN	
#include <windows.h>
#include "InputSystem.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Window/Window.hpp"


//-----------------------------------------------------------------------------------------------
unsigned char const KEYCODE_F1                   = VK_F1;
unsigned char const KEYCODE_F2                   = VK_F2;
unsigned char const KEYCODE_F3                   = VK_F3;
unsigned char const KEYCODE_F4                   = VK_F4;
unsigned char const KEYCODE_F5                   = VK_F5;
unsigned char const KEYCODE_F6                   = VK_F6;
unsigned char const KEYCODE_F7                   = VK_F7;
unsigned char const KEYCODE_F8                   = VK_F8;
unsigned char const KEYCODE_F9                   = VK_F9;
unsigned char const KEYCODE_F10                  = VK_F10;
unsigned char const KEYCODE_F11                  = VK_F11;
unsigned char const KEYCODE_F12                  = VK_F12;
unsigned char const KEYCODE_UPARROW              = VK_UP;
unsigned char const KEYCODE_DOWNARROW            = VK_DOWN;
unsigned char const KEYCODE_LEFTARROW            = VK_LEFT;
unsigned char const KEYCODE_RIGHTARROW           = VK_RIGHT;
unsigned char const KEYCODE_INSERT               = VK_INSERT;
unsigned char const KEYCODE_SPACE                = VK_SPACE;
unsigned char const KEYCODE_ESCAPE               = VK_ESCAPE;
unsigned char const KEYCODE_ENTER                = VK_RETURN;
unsigned char const KEYCODE_TILDE                = VK_OEM_3;
unsigned char const KEYCODE_BACKSPACE            = VK_BACK;
unsigned char const KEYCODE_DELETE               = VK_DELETE;
unsigned char const KEYCODE_HOME                 = VK_HOME;
unsigned char const KEYCODE_END                  = VK_END;
unsigned char const KEYCODE_SHIFT                = VK_SHIFT;
unsigned char const KEYCODE_SQUARE_OPEN_BRACKET  = VK_OEM_4;
unsigned char const KEYCODE_SQUARE_CLOSE_BRACKET = VK_OEM_6;
unsigned char const KEYCODE_COMMA                = VK_OEM_COMMA;
unsigned char const KEYCODE_PERIOD               = VK_OEM_PERIOD;
unsigned char const KEYCODE_PLUS_AND_EQUALS      = VK_OEM_PLUS;
unsigned char const KEYCODE_MINUS                = VK_OEM_MINUS;


//-----------------------------------------------------------------------------------------------
InputSystem* g_theInput = nullptr;


//-----------------------------------------------------------------------------------------------
InputSystem::InputSystem( InputConfig const& config ) : m_config( config )
{

}


//-----------------------------------------------------------------------------------------------
InputSystem::~InputSystem()
{

}


//-----------------------------------------------------------------------------------------------
void InputSystem::Startup()
{
	for ( int charcode = 0; charcode < 256; charcode++ )
	{
		m_keyStates[ charcode ].m_wasPressedLastFrame = false;
		m_keyStates[ charcode ].m_isPressed = false;
	}

	for ( int controllerNum = 0; controllerNum < NUM_XBOX_CONTROLLERS; ++controllerNum )
	{
		m_controllers[ controllerNum ].m_id = controllerNum;
		m_controllers[ controllerNum ].Reset();
	}
}


//-----------------------------------------------------------------------------------------------
void InputSystem::Shutdown()
{

}


//-----------------------------------------------------------------------------------------------
void InputSystem::BeginFrame()
{
	for ( int controllerNum = 0; controllerNum < NUM_XBOX_CONTROLLERS; ++controllerNum )
	{
		m_controllers[ controllerNum ].Update();
	}

	Window* window = Window::GetMainWindowInstance();
	if ( window->HasFocus() )
	{
		IntVec2 clientPos = GetMouseClientPosition();
		m_relativeMousePosition = ( clientPos - m_previousClientPosition );

		if ( m_currentConfig.m_isRelative == true && window->HasFocus() )
		{
			IntVec2 clientCenter = window->GetClientCenter();
			window->SetMouseClientPosition( clientCenter );
			clientPos = window->GetMouseClientPosition();
		}
		m_previousClientPosition = clientPos;
	}
}


//-----------------------------------------------------------------------------------------------
void InputSystem::EndFrame()
{
	for ( int charcode = 0; charcode < 256; charcode++ )
	{
		m_keyStates[ charcode ].m_wasPressedLastFrame = m_keyStates[ charcode ].m_isPressed;
	}

	for ( int mouseKeyNum = 0; mouseKeyNum < NUM_MOUSE_BUTTONS; mouseKeyNum++ )
	{
		m_mouseButtons[ mouseKeyNum ].m_wasPressedLastFrame = m_mouseButtons[ mouseKeyNum ].m_isPressed;
	}
}


//-----------------------------------------------------------------------------------------------
bool InputSystem::WasKeyJustPressed( unsigned char keyCode )
{
	if ( m_keyStates[ keyCode ].m_isPressed && !m_keyStates[ keyCode ].m_wasPressedLastFrame )
		return true;
	return false;
}


//-----------------------------------------------------------------------------------------------
bool InputSystem::WasKeyJustReleased( unsigned char keyCode )
{
	if ( !m_keyStates[ keyCode ].m_isPressed && m_keyStates[ keyCode ].m_wasPressedLastFrame )
		return true;
	return false;
}


//-----------------------------------------------------------------------------------------------
bool InputSystem::IsKeyDown( unsigned char keyCode )
{
	if ( m_keyStates[ keyCode ].m_isPressed )
		return true;
	return false;
}


//-----------------------------------------------------------------------------------------------
bool InputSystem::HandleKeyPressed( unsigned char keyCode )
{
	m_keyStates[ keyCode ].m_isPressed = true;
	return true;
}


//-----------------------------------------------------------------------------------------------
bool InputSystem::HandleKeyReleased( unsigned char keyCode )
{
	m_keyStates[ keyCode ].m_isPressed = false;
	return true;
}


//-----------------------------------------------------------------------------------------------
bool InputSystem::WasMouseKeyJustPressed( unsigned char keyCode )
{
	if ( m_mouseButtons[ keyCode ].m_isPressed && !m_mouseButtons[ keyCode ].m_wasPressedLastFrame )
		return true;
	return false;
}


//-----------------------------------------------------------------------------------------------
bool InputSystem::WasMouseKeyJustReleased( unsigned char keyCode )
{
	if ( !m_mouseButtons[ keyCode ].m_isPressed && m_mouseButtons[ keyCode ].m_wasPressedLastFrame )
		return true;
	return false;
}


//-----------------------------------------------------------------------------------------------
bool InputSystem::IsMouseKeyDown( unsigned char keyCode )
{
	if ( m_mouseButtons[ keyCode ].m_isPressed )
		return true;
	return false;
}


//-----------------------------------------------------------------------------------------------
bool InputSystem::HandleMouseButtonPressed( unsigned char mouseKeyCode )
{
	m_mouseButtons[ mouseKeyCode ].m_isPressed = true;
	return true;
}


//-----------------------------------------------------------------------------------------------
bool InputSystem::HandleMouseButtonReleased( unsigned char mouseKeyCode )
{
	m_mouseButtons[ mouseKeyCode ].m_isPressed = false;
	return true;
}


//-----------------------------------------------------------------------------------------------
XboxController const& InputSystem::GetController( int controllerId )
{

	if ( controllerId < 0 || controllerId > 5 )
		ERROR_AND_DIE( "Invalid Controller Requested" );

	XboxController const& newController = m_controllers[ controllerId ];
	return newController;
}


//-----------------------------------------------------------------------------------------------
void InputSystem::ClearState()
{
	for ( int keycode = 0; keycode < NUM_KEYCODES; keycode++ )
	{
		m_keyStates[ keycode ].m_wasPressedLastFrame = false;
		m_keyStates[ keycode ].m_isPressed = false;
	}

	for ( int controllerNum = 0; controllerNum < NUM_XBOX_CONTROLLERS; controllerNum++ )
	{
		m_controllers[ controllerNum ].Reset();
	}

	for ( int mouseKeyNum = 0; mouseKeyNum < NUM_MOUSE_BUTTONS; mouseKeyNum++ )
	{
		m_mouseButtons[ mouseKeyNum ].m_wasPressedLastFrame = false;
		m_mouseButtons[ mouseKeyNum ].m_isPressed = false;
	}

	m_relativeMousePosition = IntVec2( 0, 0 );

}


//-----------------------------------------------------------------------------------------------
void InputSystem::SetDefaultMousePosition()
{
	Window* window = Window::GetMainWindowInstance();
	m_relativeMousePosition = IntVec2( 0, 0 );
	m_previousClientPosition = window->GetClientCenter();
	Window::GetMainWindowInstance()->SetMouseClientPosition( m_previousClientPosition );
}


//-----------------------------------------------------------------------------------------------
void InputSystem::SetRelativeMousePosition( IntVec2 pos )
{
	m_relativeMousePosition = pos;
}


//-----------------------------------------------------------------------------------------------
void InputSystem::PushMouseConfig( MouseConfig const* config )
{
	m_priorityStack.push_back( config );
	ApplyTopMode();
}


//-----------------------------------------------------------------------------------------------
void InputSystem::PopMouseConfig( MouseConfig const* config )
{
	int configNum;
	for ( configNum = static_cast< int >( m_priorityStack.size() ) - 1; configNum >= 0; configNum-- )
	{
		if ( m_priorityStack[ configNum ] == config )
		{
			m_priorityStack.erase( m_priorityStack.begin() + configNum );
			break;
		}
	}
	ApplyTopMode();
}


//-----------------------------------------------------------------------------------------------
void InputSystem::ApplyTopMode()
{
	if ( m_priorityStack.size() == 0 )
	{
		return;
	}

	int priority = m_priorityStack[ 0 ]->m_priority;
	int configPos = 0;

	for ( int configNum = 1; configNum < m_priorityStack.size(); configNum++ )
	{
		if ( m_priorityStack[ configNum ]->m_priority >= priority )
		{
			priority = m_priorityStack[ configNum ]->m_priority;
			configPos = configNum;
		}
	}

	m_currentConfig = *( m_priorityStack[ configPos ] );

	if ( priority < 0 )
	{
		m_currentConfig = m_defaultConfig;
	}

	Window* window = Window::GetMainWindowInstance();
	window->SetCursorLock( m_currentConfig.m_isLocked );
	window->SetCursorVisibility( !m_currentConfig.m_isHidden );
}


//-----------------------------------------------------------------------------------------------
IntVec2 InputSystem::GetMouseClientPosition() const
{
	return Window::GetMainWindowInstance()->GetMouseClientPosition();
}


//-----------------------------------------------------------------------------------------------
IntVec2 InputSystem::GetMouseRelativeMovement() const
{
	return m_relativeMousePosition;
}

