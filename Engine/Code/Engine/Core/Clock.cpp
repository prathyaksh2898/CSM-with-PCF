#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/MathUtils.hpp"


//----------------------------------------------------------------------------------------------------
Clock g_theClock;


//----------------------------------------------------------------------------------------------------
Clock::Clock()
{
	Clock& systemClock = GetSystemClock();
	if ( this != &systemClock )
	{
		systemClock.AddChild( this );
		this->m_parent = &systemClock;
	}

}


//----------------------------------------------------------------------------------------------------
Clock::Clock( Clock* parent )
{
	m_parent = parent;
	parent->AddChild( this );
}


//----------------------------------------------------------------------------------------------------
Clock::~Clock()
{

}


//----------------------------------------------------------------------------------------------------
void Clock::SetParent( Clock& parent )
{
	m_parent->RemoveChild( this );
	m_parent = &( parent );
	parent.AddChild( this );

}


//----------------------------------------------------------------------------------------------------
void Clock::Pause()
{
	m_isPaused = true;

	for ( Clock* clock : m_children )
	{
		clock->Pause();
	}
}


//----------------------------------------------------------------------------------------------------
void Clock::Unpause()
{
	m_isPaused = false;
	m_timeDilation = 1.0f;

	for ( Clock* clock : m_children )
	{
		clock->Unpause();
	}
}


//----------------------------------------------------------------------------------------------------
void Clock::TogglePause()
{
	if ( m_isPaused )
	{
		Unpause();
	}

	else
	{
		Pause();
	}
}


//----------------------------------------------------------------------------------------------------
void Clock::StepFrame()
{
	m_pauseAfterFrame = true;

	for ( Clock* child : m_children )
	{
		child->StepFrame();
	}
}


//----------------------------------------------------------------------------------------------------
void Clock::SetTimeDilation( double dilationAmount )
{
	m_timeDilation = dilationAmount;
}


//----------------------------------------------------------------------------------------------------
void Clock::Reset( bool resetChildren /*= true */ )
{
	m_timeDilation       = 1.0f;
	m_lastUpdateTime     = 0.0f;
	m_totalSecondsPassed = 0.0f;
	m_frameCount         = 0;
	m_frameDeltaSeconds  = 0.0f;
	m_isPaused           = false;
	m_pauseAfterFrame    = false;

	if ( resetChildren )
	{
		for ( Clock* child : m_children )
		{
			child->Reset( resetChildren );
		}
	}
}


//----------------------------------------------------------------------------------------------------
void Clock::RemoveSelfFromParent()
{
	m_parent->RemoveChild( this );
}


//----------------------------------------------------------------------------------------------------
void Clock::SystemBeginFrame()
{
	g_theClock.Tick();
}


//----------------------------------------------------------------------------------------------------
Clock& Clock::GetSystemClock()
{
	return g_theClock;
}


//----------------------------------------------------------------------------------------------------
void Clock::Tick()
{
	m_totalSecondsPassed = GetCurrentTimeSeconds();
	
	m_frameDeltaSeconds = m_totalSecondsPassed - m_lastUpdateTime;
	m_lastUpdateTime = m_totalSecondsPassed;

	if ( m_frameDeltaSeconds > 0.1 )
	{
		m_frameDeltaSeconds = 0.1;
	}

	m_frameCount++;

	for ( Clock* child : m_children )
	{
		child->AdvanceTime( m_frameDeltaSeconds );
	}
}


//----------------------------------------------------------------------------------------------------
void Clock::AdvanceTime( double deltaTimeSeconds )
{
	deltaTimeSeconds = m_timeDilation * deltaTimeSeconds;

	if ( IsPaused() )
	{
		m_frameCount--;
		deltaTimeSeconds = 0.0;
	}

	m_frameDeltaSeconds = deltaTimeSeconds;

	m_lastUpdateTime = m_totalSecondsPassed;
	m_totalSecondsPassed += m_frameDeltaSeconds;
	m_frameCount++;

	for ( Clock* child : m_children )
	{
		child->AdvanceTime( deltaTimeSeconds );
	}

	if ( m_pauseAfterFrame )
	{
		m_pauseAfterFrame = false;
		m_isPaused = true;
	}
}


//----------------------------------------------------------------------------------------------------
void Clock::AddChild( Clock* childClock )
{
	m_children.push_back( childClock );
}


//----------------------------------------------------------------------------------------------------
void Clock::RemoveChild( Clock* childClock )
{
	for ( int childNum = 0; childNum < static_cast< int >( m_children.size() ); childNum++ )
	{
		if ( m_children[ childNum ] == childClock )
		{
			m_children.erase( m_children.begin() + childNum );
		}
	}
}

