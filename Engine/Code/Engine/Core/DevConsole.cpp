#include "DevConsole.hpp"

#include "Engine/Core/Clock.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/Stopwatch.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/VertexData/Vertex_PCU.hpp"
#include "Engine/Renderer/VertexData/VertexUtils.hpp"

#include <vector>

//----------------------------------------------------------------------------------------------------
DevConsole* g_theConsole = nullptr;


//----------------------------------------------------------------------------------------------------
DevConsole::DevConsole(DevConsoleConfig const& config)
{
	m_config = config;
}


//----------------------------------------------------------------------------------------------------
DevConsole::~DevConsole()
{

}


//----------------------------------------------------------------------------------------------------
void DevConsole::Startup()
{
	SubscribeEventCallbackFunction( "HandleCharInput", DevConsole::HandleCharInputFromWindows );
	SubscribeEventCallbackFunction( "HandleKeyDownInput", DevConsole::HandleKeyDownInputFromWindows );
	SubscribeEventCallbackFunction( "clear", DevConsole::ClearDevConsole );

	m_mouseConfig.m_priority = 100;
	m_mouseConfig.m_isHidden = false;
	m_mouseConfig.m_isLocked = false;
	m_mouseConfig.m_isRelative = false;

	m_consoleClock = new Clock();
	m_caretStopwatch = new Stopwatch( *(m_consoleClock), 0.50 );

}


//----------------------------------------------------------------------------------------------------
void DevConsole::BeginFrame()
{
	m_frameNumber++;
}


//----------------------------------------------------------------------------------------------------
void DevConsole::EndFrame()
{

}


//----------------------------------------------------------------------------------------------------
void DevConsole::Shutdown()
{

}


//----------------------------------------------------------------------------------------------------
bool DevConsole::HandleCharInputFromWindows( EventArgs& args )
{
	if (g_theConsole->m_mode != DevConsoleMode::HIDDEN)
	{
		g_theConsole->m_handlingInput = true;
		g_theConsole->m_caretVisible = true;

		g_theConsole->m_caretStopwatch->Restart();

		int charInCommand = static_cast<int>(args.GetValue( "KeyValue", 0 ));

		if (charInCommand == '`' ||
			charInCommand == 13 ||
			charInCommand == 27 ||
			charInCommand == 8)
			return false;


		if (charInCommand != '\0')
		{
			g_theConsole->m_activeCommand.insert( g_theConsole->m_caretLocation, 1, static_cast<char>(charInCommand) );
			g_theConsole->m_caretLocation++;
			return true;
		}
	}
	return false;
}


//----------------------------------------------------------------------------------------------------
bool DevConsole::HandleKeyDownInputFromWindows( EventArgs& args )
{
	if (g_theConsole->m_mode != DevConsoleMode::HIDDEN)
	{
		g_theConsole->m_handlingInput = true;
		g_theConsole->m_caretVisible = true;
		g_theConsole->m_caretStopwatch->Restart();


		unsigned char charInCommand = static_cast<unsigned char>(args.GetValue( "KeyValue", 0 ));

		if (charInCommand == KEYCODE_BACKSPACE)
		{
			if (g_theConsole->m_caretLocation == 0)
				return true;

			g_theConsole->m_activeCommand.erase( static_cast<size_t>(g_theConsole->m_caretLocation) - 1, 1 );
			g_theConsole->m_caretLocation--;

			return true;
		}

		else if (charInCommand == KEYCODE_ENTER)
		{
			if (g_theConsole->m_activeCommand.empty())
			{
				return true;
			}

			std::string command = g_theConsole->m_activeCommand;
			
			g_theConsole->Execute( command );

			if (g_theConsole->m_commandHistory.size() == 0)
				g_theConsole->m_commandHistory.push_back( command );


			else if(g_theConsole->m_commandHistory[g_theConsole->m_commandHistory.size() - 1] != command)
			{
				g_theConsole->m_commandHistory.push_back( command );

				if (g_theConsole->m_commandHistory.size() > 128)
				{
					g_theConsole->m_commandHistory.erase( g_theConsole->m_commandHistory.begin() );
				}
			}

			g_theConsole->m_activeCommand = "";
			g_theConsole->m_caretLocation = 0;
			g_theConsole->m_commandHistoryLocation = 0;

			return true;
		}

		else if (charInCommand == KEYCODE_ESCAPE)
		{
			if (g_theConsole->m_activeCommand.empty())
			{
				g_theConsole->SetMode( DevConsoleMode::HIDDEN );
			}

			else
			{
				g_theConsole->m_activeCommand = "";
				g_theConsole->m_caretLocation = 0;
			}

			return true;
		}

		else if (charInCommand == KEYCODE_TILDE)
		{
			g_theConsole->SetMode( DevConsoleMode::HIDDEN );
			return true;
		}

		else if (charInCommand == KEYCODE_DELETE)
		{
			if (g_theConsole->m_caretLocation == g_theConsole->m_activeCommand.size())
				return true;

			g_theConsole->m_activeCommand.erase( static_cast<size_t>(g_theConsole->m_caretLocation), 1 );
		}

		else if (charInCommand == KEYCODE_RIGHTARROW)
		{
			if (g_theConsole->m_caretLocation == g_theConsole->m_activeCommand.size())
				return true;
			
			g_theConsole->m_caretLocation++;
			return true;
		}

		else if (charInCommand == KEYCODE_LEFTARROW)
		{
			if (g_theConsole->m_caretLocation == 0)
				return true;

			g_theConsole->m_caretLocation--;
			return true;
		}

		else if (charInCommand == KEYCODE_UPARROW)
		{
			if (g_theConsole->m_commandHistory.size() == 0)
				return true;
			
			g_theConsole->m_commandHistoryLocation++;

			if (g_theConsole->m_commandHistoryLocation > g_theConsole->m_commandHistory.size())
				g_theConsole->m_commandHistoryLocation--;

			g_theConsole->m_activeCommand = g_theConsole->m_commandHistory[g_theConsole->m_commandHistory.size() - g_theConsole->m_commandHistoryLocation];
			g_theConsole->m_caretLocation = 0;
			return true;
		}

		else if (charInCommand == KEYCODE_DOWNARROW)
		{
			if (g_theConsole->m_commandHistory.size() == 0)
				return true;

			g_theConsole->m_commandHistoryLocation--;

			if (g_theConsole->m_commandHistoryLocation < 0)
				g_theConsole->m_commandHistoryLocation++;

			if (g_theConsole->m_commandHistoryLocation == 0)
			{
				g_theConsole->m_activeCommand = "";

			}

			else
			{
				g_theConsole->m_activeCommand = g_theConsole->m_commandHistory[g_theConsole->m_commandHistory.size() - g_theConsole->m_commandHistoryLocation];
			}

			g_theConsole->m_caretLocation = 0;
			return true;
		}

		else if (charInCommand == KEYCODE_HOME)
		{
			g_theConsole->m_caretLocation = 0;
			return true;
		}

		else if (charInCommand == KEYCODE_END)
		{
			g_theConsole->m_caretLocation = static_cast<int>(g_theConsole->m_activeCommand.size());
			return true;
		}

		return false;
	}

	return false;
}


//----------------------------------------------------------------------------------------------------
bool DevConsole::ClearDevConsole( EventArgs& args )
{
	UNUSED( args );

	if (g_theConsole != nullptr)
	{
		g_theConsole->m_lines.clear();
		return true;
	}

	return false;
}


//----------------------------------------------------------------------------------------------------
void DevConsole::Update()
{
	if (IsProcessingInput())
	{
		m_config.m_defaultInputSystem->ClearState();
		if (m_mode == DevConsoleMode::HIDDEN)
		{
			m_handlingInput = false;
		}
	}

	if (m_mode != DevConsoleMode::HIDDEN)
	{
		m_config.m_defaultInputSystem->SetRelativeMousePosition(IntVec2(0, 0));
		m_config.m_defaultInputSystem->ClearState();
	}

	if ( m_caretStopwatch->DecrementAll() > 0 )
	{
		m_caretVisible = !m_caretVisible;
	}
}


//----------------------------------------------------------------------------------------------------
void DevConsole::Execute(std::string const& consoleCommandText)
{
	Strings strings = SplitStringOnDelimiter(consoleCommandText, ' ');
	std::string eventName = strings[0];
	EventArgs args;
	
	if (g_theEventSystem->IsValidEvent( eventName ))
	{
		AddLine( DevConsole::SUCCESS, Stringf( "Executing command: %s", eventName.c_str() ) );
	}

	for (int argNum = 1; argNum < strings.size(); argNum++) 
	{
		Strings argumentKeyValue;
		argumentKeyValue = SplitStringOnDelimiter(strings[argNum], '=');

		GUARANTEE_OR_DIE(static_cast<int>(argumentKeyValue.size()) == 2, "Arguments Supplied in Wrong Format!");
		
		args.SetValue(argumentKeyValue[0], argumentKeyValue[1]);
	}

	FireEvent(eventName, args);
}


//----------------------------------------------------------------------------------------------------
void DevConsole::AddLine(Rgba8 const& color, std::string const& text)
{
	DevConsoleLine newLine;
	float time = static_cast<float>(m_consoleClock->GetTotalSeconds());
	newLine.text = Stringf("[%0.3f][%d] %s", time, m_frameNumber, text.c_str());
	newLine.color = color;
	m_devConsoleLock.lock();
	m_lines.push_back(newLine);
	m_devConsoleLock.unlock();
}


//----------------------------------------------------------------------------------------------------
void DevConsole::Render(AABB2 const& bounds, Renderer* rendererOverrride /*= nullptr*/) const
{
	if (m_mode == DevConsoleMode::HIDDEN)
		return;

	Renderer* consoleRenderer = m_config.m_defaultRenderer;
	
	if (rendererOverrride)
	{
		consoleRenderer = rendererOverrride; 
	}

	std::string fontFilePath = "Data/Fonts/" + m_config.m_defaultFontName;
	BitmapFont* font = consoleRenderer->CreateOrGetBitmapFontFromFile(fontFilePath.c_str());
	Render_OpenFull(bounds, *consoleRenderer, *font, m_config.m_defaultFontAspect);
}


//----------------------------------------------------------------------------------------------------
DevConsoleMode DevConsole::GetMode() const
{
	return m_mode;
}


//----------------------------------------------------------------------------------------------------
void DevConsole::SetMode(DevConsoleMode mode)
{
	if (mode == DevConsoleMode::HIDDEN)
	{
		if(m_config.m_defaultInputSystem != nullptr)
		{
			m_config.m_defaultInputSystem->ClearState();
		}

		m_config.m_defaultInputSystem->PopMouseConfig( &m_mouseConfig );
	}

	else
	{
		m_caretStopwatch->Start( 0.5 );
		m_config.m_defaultInputSystem->PushMouseConfig( &m_mouseConfig );
	}

	m_mode = mode;


}


//----------------------------------------------------------------------------------------------------
void DevConsole::ToggleMode(DevConsoleMode mode)
{

	if (m_mode == mode) 
	{
		m_mode = DevConsoleMode::HIDDEN;
	}
	
	else
	{
		//m_caretStopwatch->Start( 0.5 );
		m_mode = mode;
	}
}


//----------------------------------------------------------------------------------------------------
bool DevConsole::IsProcessingInput()
{
	if (m_handlingInput)
		return true;
	
	return false;
}


//----------------------------------------------------------------------------------------------------
Rgba8 const DevConsole::ERROR = Rgba8( 255, 0, 0 );
Rgba8 const DevConsole::SUCCESS = Rgba8( 0, 255, 0 );
Rgba8 const DevConsole::WARNING = Rgba8( 255, 255, 0 );
Rgba8 const DevConsole::INFO_MAJOR = Rgba8( 255, 160, 0 );
Rgba8 const DevConsole::INFO_MINOR = Rgba8( 255, 255, 255 );


//----------------------------------------------------------------------------------------------------
void DevConsole::Render_OpenFull(AABB2 const& bounds, Renderer& renderer, BitmapFont& font, float fontAspect /*= 1.f*/) const
{
	RasterState state;
	state.m_cullmode = CullMode::BACK;
	state.m_fillMode = FillMode::SOLID;
	state.m_windingOrder = WindingOrder::COUNTER_CLOCKWISE;
	renderer.SetRasterState( state );

	Vec2 displayDimensions = bounds.GetDimensions();
	Vec2 lineDimension = Vec2(displayDimensions.x, displayDimensions.y / m_config.m_linesOnScreen);

	std::vector<Vertex_PCU> backgroundInputVerts;
	AABB2 backgroundInputBox;
	backgroundInputBox.m_mins = bounds.m_mins;
	backgroundInputBox.m_maxs = bounds.m_mins + Vec2( bounds.m_maxs.x - bounds.m_mins.x, lineDimension.y * 1.5f);
	AddVertsForAABB2D( backgroundInputVerts, backgroundInputBox, Rgba8( 50, 50, 50, 200 ) );
	renderer.BindTexture( nullptr );
	renderer.DrawVertexArray( static_cast<int>(backgroundInputVerts.size()), backgroundInputVerts.data() );

	renderer.BindTexture( &font.GetTexture() );
	std::vector<Vertex_PCU> inputTextVerts;
	float lineHeight = (backgroundInputBox.m_maxs.y - backgroundInputBox.m_mins.y) * 0.75f;
	font.AddVertsForTextInBox2D( inputTextVerts, backgroundInputBox, lineHeight, m_activeCommand, Rgba8::WHITE, fontAspect, Vec2( 0.f, 0.5f ));
	renderer.BindTexture(&font.GetTexture());
	renderer.DrawVertexArray( static_cast<int>(inputTextVerts.size()), inputTextVerts.data() );

	AABB2 caret;
	std::vector<Vertex_PCU> vertsForCaret;
	caret.m_mins = backgroundInputBox.m_mins;
	caret.m_maxs = backgroundInputBox.m_maxs;
	caret.m_maxs.x = caret.m_mins.x + 0.0025f * (bounds.m_maxs.x - bounds.m_mins.x);
	
	caret.Translate( Vec2(m_caretLocation * fontAspect * lineHeight ,0.0f) );
	AddVertsForAABB2D( vertsForCaret, caret, Rgba8( 255, 255, 255, m_caretVisible ? 255 : 0 ) );
	renderer.BindTexture( nullptr );
	renderer.DrawVertexArray( static_cast<int>(vertsForCaret.size()), vertsForCaret.data() );

	std::vector<Vertex_PCU> backgroundVerts;
	AABB2 backgroundBox = bounds;
	backgroundBox.m_mins.y = backgroundInputBox.m_maxs.y;
	AddVertsForAABB2D(backgroundVerts, backgroundBox, Rgba8(125, 125, 125, 200));
	renderer.BindTexture(nullptr);
	renderer.DrawVertexArray(static_cast<int>(backgroundVerts.size()), backgroundVerts.data());

	int numLines = static_cast<int>(m_lines.size());

	renderer.BindTexture(&font.GetTexture());

	int startLine = 0;
	
	if (numLines > m_config.m_linesOnScreen + 3)
	{
		startLine = numLines - static_cast<int>(m_config.m_linesOnScreen) - 3;
	}

	std::vector<Vertex_PCU> textVerts;
	
	for (int lineNum = startLine; lineNum < numLines; lineNum++)
	{
		AABB2 boxForLine;
		boxForLine.m_mins = backgroundBox.m_mins + Vec2(0.f, lineDimension.y * (numLines - lineNum - 1));
		boxForLine.m_maxs = boxForLine.m_mins + lineDimension;
		
		Rgba8 lineColor = m_lines[lineNum].color;
		std::string lineText = m_lines[lineNum].text;

		font.AddVertsForTextInBox2D(textVerts, boxForLine, lineDimension.y, lineText, lineColor, fontAspect, Vec2(0.f, 0.5f));
	}
	renderer.DrawVertexArray(static_cast<int>(textVerts.size()), textVerts.data());
	

}
