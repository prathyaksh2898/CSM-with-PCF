#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/IntVec2.hpp"
#include <string>

//-----------------------------------------------------------------------------------------------
class InputSystem;

//-----------------------------------------------------------------------------------------------
struct WindowConfig
{
	InputSystem* m_inputSystem = nullptr;
	float m_aspectRatio = 2.f;
	std::string m_windowTitle = "UNNAMED APPLICATION";
};


//-----------------------------------------------------------------------------------------------
class Window {
public:
	Window(WindowConfig const& config);

	void Startup();
	void BeginFrame();
	void EndFrame();
	void Shutdown();

	WindowConfig const& GetConfig() const;
	void* GetDC() const;
	void* GetHWND() const;
	
	int GetClientWidth() const;
	int GetClientHeight() const;
	
	static Window* GetMainWindowInstance();

	bool    HasFocus();
	void    OnFocusChanged( bool hasFocus );

	void    SetCursorVisibility(bool isVisible);
	void    SetCursorLock(bool lock);

	void    ShowMouse( bool show );
	bool    IsMouseVisibleInternal() const;
	void    LockMouse( bool lock );
	bool    IsMouseLocked() const;

	IntVec2 GetMouseDesktopPosition() const;    
	void    SetMouseDesktopPosition( IntVec2 const& pos );

	IntVec2 GetMouseClientPosition() const;     
	void    SetMouseClientPosition( IntVec2 const& pos );

	IntVec2 GetClientCenter();

protected:
	void CreateOSWindow();
	void RunMessagePump();

protected:
	static Window* s_theWindow;

protected:
	WindowConfig m_config;
	
	IntVec2      m_clientDimensions = IntVec2(0, 0);
			     
	bool         m_isCursorVisible       = true;
    bool         m_isCursorLocked        = false;
								    	 
	void*        m_hwnd                  = nullptr;
	void*        m_dc                    = nullptr;
};