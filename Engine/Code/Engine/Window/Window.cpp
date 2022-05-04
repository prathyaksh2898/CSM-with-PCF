#include "Window.hpp"

#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/IntVec2.hpp"

#include "ThirdParty/imgui/imgui_impl_win32.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>


//-----------------------------------------------------------------------------------------------
Window* g_theWindow = nullptr;
Window* Window::s_theWindow = nullptr;


//-----------------------------------------------------------------------------------------------
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );


//-----------------------------------------------------------------------------------------------
LRESULT CALLBACK WindowsMessageHandlingProcedure( HWND windowHandle, UINT wmMessageCode, WPARAM wParam, LPARAM lParam )
{

	if ( ImGui_ImplWin32_WndProcHandler( windowHandle, wmMessageCode, wParam, lParam ) )
	{
		return true;
	}

	//if ( ImGui::GetCurrentContext() && ImGui::GetIO().WantCaptureMouse && ( !ImGui::IsAnyItemActive() || ImGui::IsAnyItemFocused() ) )
	//{
	//	return true;
	//}

	Window* window = Window::GetMainWindowInstance();
	GUARANTEE_OR_DIE( window != nullptr, "Window was null!" );

	InputSystem* input = window->GetConfig().m_inputSystem;
	GUARANTEE_OR_DIE( window != nullptr, "Window's InputSystem pointer was null!" );

	switch ( wmMessageCode )
	{

		case WM_CLOSE:
		{
			FireEvent( "Quit" );
			return 0;
		}

		case WM_CHAR:
		{
			int asKey = static_cast< int >( wParam );

			EventArgs keyArg;
			keyArg.SetValue( "KeyValue", Stringf( "%d", asKey ) );

			FireEvent( "HandleCharInput", keyArg );

			break;
		}

		case WM_LBUTTONDOWN:
		{
			bool btnDown = ( wParam & MK_LBUTTON ) != 0;
			bool wasConsumed = false;

			if ( input && btnDown )
			{
				wasConsumed = input->HandleMouseButtonPressed( 0 );
				if ( wasConsumed )
				{
					return 0;
				}
			}
			break;
		}

		case WM_RBUTTONDOWN:
		{
			bool btnDown = ( wParam & MK_RBUTTON ) != 0;
			bool wasConsumed = false;

			if ( input && btnDown )
			{
				wasConsumed = input->HandleMouseButtonPressed( 1 );
				if ( wasConsumed )
				{
					return 0;
				}
			}
			break;
		}

		case WM_MBUTTONDOWN:
		{
			bool btnDown = ( wParam & MK_MBUTTON ) != 0;
			bool wasConsumed = false;

			if ( input && btnDown )
			{
				wasConsumed = input->HandleMouseButtonPressed( 2 );
				if ( wasConsumed )
				{
					return 0;
				}
			}
			break;
		}

		case WM_LBUTTONUP:
		{
			bool wasConsumed = false;

			if ( input )
			{
				wasConsumed = input->HandleMouseButtonReleased( 0 );
				if ( wasConsumed )
				{
					return 0;
				}
			}
			break;
		}

		case WM_RBUTTONUP:
		{
			bool wasConsumed = false;

			if ( input )
			{
				wasConsumed = input->HandleMouseButtonReleased( 1 );
				if ( wasConsumed )
				{
					return 0;
				}
			}
			break;

		}

		case WM_MBUTTONUP:
		{
			bool wasConsumed = false;

			if ( input )
			{
				wasConsumed = input->HandleMouseButtonReleased( 2 );
				if ( wasConsumed )
				{
					return 0;
				}
			}
			break;
		}

		case WM_KEYDOWN:
		{
			unsigned char asKey = ( unsigned char ) wParam;
			bool wasConsumed = false;

			EventArgs keyArg;
			keyArg.SetValue( "KeyValue", Stringf( "%u", asKey ) );

			FireEvent( "HandleKeyDownInput", keyArg );

			if ( input )
			{
				wasConsumed = input->HandleKeyPressed( asKey );
				if ( wasConsumed )
				{
					return 0;
				}
			}
			break;
		}

		case WM_KEYUP:
		{
			unsigned char asKey = ( unsigned char ) wParam;
			bool wasConsumed = false;
			if ( input )
			{
				wasConsumed = input->HandleKeyReleased( asKey );
				if ( wasConsumed )
				{
					return 0;
				}
			}
			break;
		}

		case WM_ACTIVATE:
		{
			bool isActive = wParam != WA_INACTIVE;

			if ( isActive )
			{
				Window::GetMainWindowInstance()->OnFocusChanged( true );
			}

			else
			{
				Window::GetMainWindowInstance()->OnFocusChanged( false );
			}

			return 0;
		}
	}

	return DefWindowProc( windowHandle, wmMessageCode, wParam, lParam );
}


//-----------------------------------------------------------------------------------------------
Window::Window( WindowConfig const& config )
	: m_config( config )
{
	s_theWindow = this;
}


//-----------------------------------------------------------------------------------------------
void Window::Startup()
{
	CreateOSWindow();
}


//-----------------------------------------------------------------------------------------------
void Window::BeginFrame()
{
	RunMessagePump();
}


//-----------------------------------------------------------------------------------------------
void Window::EndFrame()
{

}


//-----------------------------------------------------------------------------------------------
void Window::Shutdown()
{

}


//-----------------------------------------------------------------------------------------------
WindowConfig const& Window::GetConfig() const
{
	return m_config;
}


//-----------------------------------------------------------------------------------------------
void* Window::GetDC() const
{
	return m_dc;
}


//-----------------------------------------------------------------------------------------------
void* Window::GetHWND() const
{
	return m_hwnd;
}


//-----------------------------------------------------------------------------------------------
int Window::GetClientWidth() const
{
	return m_clientDimensions.x;
}


//-----------------------------------------------------------------------------------------------
int Window::GetClientHeight() const
{
	return m_clientDimensions.y;
}

//-----------------------------------------------------------------------------------------------
Window* Window::GetMainWindowInstance()
{
	return s_theWindow;
}


//-----------------------------------------------------------------------------------------------
bool Window::HasFocus()
{
	HWND hwnd = ::GetForegroundWindow();
	return ( hwnd == m_hwnd );
}


//-----------------------------------------------------------------------------------------------
void Window::OnFocusChanged( bool hasFocus )
{
	if ( hasFocus )
	{
		ShowMouse( m_isCursorVisible );
		LockMouse( m_isCursorLocked );
		SetMouseClientPosition( GetClientCenter() );
	}

	else
	{
		ShowMouse( true );
		LockMouse( false );
		m_config.m_inputSystem->ClearState();
	}
}


//-----------------------------------------------------------------------------------------------
void Window::SetCursorVisibility( bool isVisble )
{
	if ( m_isCursorVisible != isVisble )
	{
		m_isCursorVisible = isVisble;
		if ( HasFocus() )
		{
			ShowMouse( isVisble );
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Window::SetCursorLock( bool lock )
{
	if ( m_isCursorLocked != lock )
	{
		m_isCursorLocked = lock;
		if ( HasFocus() )
		{
			LockMouse( lock );
		}
	}
}


//-----------------------------------------------------------------------------------------------
void Window::ShowMouse( bool show )
{
	int counter;

	if ( IsMouseVisibleInternal() )
	{
		if ( !show )
		{
			do
			{
				counter = ::ShowCursor( FALSE );
			} while ( counter >= 0 );
		}
	}

	else
	{
		if ( show )
		{
			do
			{
				counter = ::ShowCursor( TRUE );
			} while ( counter <= 0 );
		}
	}
}


//-----------------------------------------------------------------------------------------------
bool Window::IsMouseVisibleInternal() const
{
	CURSORINFO cursorInfo;
	cursorInfo.cbSize = sizeof( cursorInfo );
	BOOL gotInfo = ::GetCursorInfo( &cursorInfo );

	bool cursorIsVisible = !gotInfo || ( ( cursorInfo.flags & CURSOR_SHOWING ) != 0 );
	return cursorIsVisible;
}


//-----------------------------------------------------------------------------------------------
void Window::LockMouse( bool lock )
{
	if ( lock )
	{
		HWND hwnd = reinterpret_cast< HWND >( m_hwnd );
		RECT clientRect;
		POINT origin;
		origin.x = 0;
		origin.y = 0;
		::GetClientRect( hwnd, &clientRect );
		::ClientToScreen( hwnd, &origin );

		clientRect.left += origin.x;
		clientRect.right += origin.x;
		clientRect.top += origin.y;
		clientRect.bottom += origin.y;

		::ClipCursor( &clientRect );
	}

	else
	{
		::ClipCursor( nullptr );
	}
}


//-----------------------------------------------------------------------------------------------
bool Window::IsMouseLocked() const
{
	return m_isCursorLocked;
}


//-----------------------------------------------------------------------------------------------
IntVec2 Window::GetMouseDesktopPosition() const
{
	POINT mousePos;
	::GetCursorPos( &mousePos );

	return IntVec2( mousePos.x, mousePos.y );
}


//-----------------------------------------------------------------------------------------------
void Window::SetMouseDesktopPosition( IntVec2 const& pos )
{
	::SetCursorPos( pos.x, pos.y );
}


//-----------------------------------------------------------------------------------------------
IntVec2 Window::GetMouseClientPosition() const
{
	IntVec2 desktopPos = GetMouseDesktopPosition();

	POINT point;
	point.x = desktopPos.x;
	point.y = desktopPos.y;

	::ScreenToClient( reinterpret_cast< HWND >( m_hwnd ), &point );

	return IntVec2( point.x, point.y );
}


//-----------------------------------------------------------------------------------------------
void Window::SetMouseClientPosition( IntVec2 const& pos )
{
	POINT point;
	point.x = pos.x;
	point.y = pos.y;
	::ClientToScreen( reinterpret_cast< HWND >( m_hwnd ), &point );

	SetMouseDesktopPosition( IntVec2( point.x, point.y ) );
}


//-----------------------------------------------------------------------------------------------
IntVec2 Window::GetClientCenter()
{
	if ( m_hwnd == nullptr )
		return IntVec2( 0, 0 );
	HWND hwnd = reinterpret_cast< HWND >( m_hwnd );
	RECT clientRect;
	::GetClientRect( hwnd, &clientRect );

	return IntVec2( ( clientRect.right - clientRect.left ) / 2, ( clientRect.bottom - clientRect.top ) / 2 );
}


//-----------------------------------------------------------------------------------------------
void Window::CreateOSWindow()
{
	HMODULE applicationInstanceHandle = GetModuleHandle( NULL );
	float clientAspect = m_config.m_aspectRatio;

	WNDCLASSEX windowClassDescription;
	memset( &windowClassDescription, 0, sizeof( windowClassDescription ) );
	windowClassDescription.cbSize = sizeof( windowClassDescription );
	windowClassDescription.style = CS_OWNDC;
	windowClassDescription.lpfnWndProc = static_cast< WNDPROC >( WindowsMessageHandlingProcedure ); // Register our Windows message-handling function
	windowClassDescription.hInstance = applicationInstanceHandle;
	windowClassDescription.hIcon = NULL;
	windowClassDescription.hCursor = NULL;
	windowClassDescription.lpszClassName = TEXT( "Simple Window Class" );
	RegisterClassEx( &windowClassDescription );

	const DWORD windowStyleFlags = WS_CAPTION | WS_BORDER | WS_SYSMENU | WS_OVERLAPPED;
	const DWORD windowStyleExFlags = WS_EX_APPWINDOW;

	RECT desktopRect;
	HWND desktopWindowHandle = GetDesktopWindow();
	GetClientRect( desktopWindowHandle, &desktopRect );
	float desktopWidth = ( float ) ( desktopRect.right - desktopRect.left );
	float desktopHeight = ( float ) ( desktopRect.bottom - desktopRect.top );
	float desktopAspect = desktopWidth / desktopHeight;

	//constexpr float maxClientFractionOfDesktop = 0.90f;
	float clientWidth =  1600;
	float clientHeight = 800;

	if ( clientAspect > desktopAspect )
	{
		clientHeight = clientWidth / clientAspect;
	}
	else
	{
		clientWidth = clientHeight * clientAspect;
	}

	m_clientDimensions.x = static_cast< int >( clientWidth );
	m_clientDimensions.y = static_cast< int >( clientHeight );

	float clientMarginX = 0.5f * ( desktopWidth - clientWidth );
	float clientMarginY = 0.5f * ( desktopHeight - clientHeight );
	RECT clientRect;
	clientRect.left = ( int ) clientMarginX;
	clientRect.right = clientRect.left + ( int ) clientWidth;
	clientRect.top = ( int ) clientMarginY;
	clientRect.bottom = clientRect.top + ( int ) clientHeight;

	RECT windowRect = clientRect;
	AdjustWindowRectEx( &windowRect, windowStyleFlags, FALSE, windowStyleExFlags );

	WCHAR windowTitle[ 1024 ];
	MultiByteToWideChar( GetACP(), 0, m_config.m_windowTitle.c_str(), -1, windowTitle, sizeof( windowTitle ) / sizeof( windowTitle[ 0 ] ) );
	HWND hwnd = CreateWindowEx(
		windowStyleExFlags,
		windowClassDescription.lpszClassName,
		windowTitle,
		windowStyleFlags,
		windowRect.left,
		windowRect.top,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		NULL,
		NULL,
		applicationInstanceHandle,
		NULL );

	ShowWindow( hwnd, SW_SHOW );
	SetForegroundWindow( hwnd );
	SetFocus( hwnd );

	HDC dc = ::GetDC( hwnd );
	HCURSOR cursor = LoadCursor( NULL, IDC_ARROW );
	SetCursor( cursor );

	m_hwnd = hwnd;
	m_dc = dc;
}


//-----------------------------------------------------------------------------------------------
void Window::RunMessagePump()
{
	MSG queuedMessage;
	for ( ;; )
	{
		const BOOL wasMessagePresent = PeekMessage( &queuedMessage, NULL, 0, 0, PM_REMOVE );
		if ( !wasMessagePresent )
		{
			break;
		}
		TranslateMessage( &queuedMessage );
		DispatchMessage( &queuedMessage );
	}
}


