#include "Engine/Core/Stopwatch.hpp"
#include "Engine/Core/Clock.hpp"


//-----------------------------------------------------------------------------------------------
Stopwatch::Stopwatch()
{

}


//-----------------------------------------------------------------------------------------------
Stopwatch::Stopwatch( double secondDuration )
{
	m_sourceClock = &(Clock::GetSystemClock());
	m_duration = secondDuration;

	m_startTime = m_sourceClock->GetTotalSeconds();
}


//-----------------------------------------------------------------------------------------------
Stopwatch::Stopwatch( Clock const& sourceClock, double secondDuration )
{
	m_sourceClock = &( sourceClock );
	m_duration = secondDuration;

	m_startTime = m_sourceClock->GetTotalSeconds();
}


//-----------------------------------------------------------------------------------------------
void Stopwatch::Start( Clock const& sourceClock, double seconds )
{
	m_sourceClock = &( sourceClock );
	m_duration = seconds;

	m_startTime = m_sourceClock->GetTotalSeconds();
}


//-----------------------------------------------------------------------------------------------
void Stopwatch::Start()
{
	if ( m_sourceClock == nullptr )
	{
		m_sourceClock = &( Clock::GetSystemClock() );
	}

	m_startTime = m_sourceClock->GetTotalSeconds();
}


//-----------------------------------------------------------------------------------------------
void Stopwatch::Start( double seconds )
{
	if ( m_sourceClock == nullptr )
	{
		m_sourceClock = &( Clock::GetSystemClock() );
	}
	
	m_duration = seconds;
	m_startTime = m_sourceClock->GetTotalSeconds();
}


//-----------------------------------------------------------------------------------------------
void Stopwatch::SetClock( Clock const& sourceClock )
{
	double elapsedTime = 0.0f;

	if ( m_sourceClock != nullptr )
	{
		elapsedTime = m_sourceClock->GetTotalSeconds() - m_startTime;
	}
	
	m_sourceClock = &( sourceClock );
	m_startTime = m_sourceClock->GetTotalSeconds() - elapsedTime;
}


//-----------------------------------------------------------------------------------------------
void Stopwatch::Restart()
{
	m_startTime = m_sourceClock->GetTotalSeconds();
	m_isPaused = false;
	m_isStopped = false;
}


//-----------------------------------------------------------------------------------------------
void Stopwatch::Stop()
{
	m_startTime = 0;
	m_duration = 0;
	m_isStopped = 0;
}


//-----------------------------------------------------------------------------------------------
double Stopwatch::GetElapsedSeconds() const
{
	if ( m_isStopped )
	{
		return 0.0;
	}

	double elapsedTime = m_sourceClock->GetTotalSeconds() - m_startTime;

	if ( m_isPaused )
	{
		elapsedTime = m_pausedDelta + m_startTime;
	}

	return elapsedTime;
}


//-----------------------------------------------------------------------------------------------
float Stopwatch::GetElapsedFraction() const
{
	if ( m_isStopped )
	{
		return 0.0f;
	}

	double elapsedTime = m_sourceClock->GetTotalSeconds() - m_startTime;

	if ( m_isPaused )
	{
		elapsedTime = m_pausedDelta + m_startTime;
	}

	return static_cast< float >( elapsedTime / m_duration );
}


//-----------------------------------------------------------------------------------------------
bool Stopwatch::IsStopped() const
{
	return m_isStopped;
}


//-----------------------------------------------------------------------------------------------
bool Stopwatch::HasElapsed() const
{
	double elapsedTime = m_sourceClock->GetTotalSeconds() - m_startTime;

	if ( m_isStopped )
	{
		return false;
	}


	if ( m_isPaused )
	{
		elapsedTime = m_pausedDelta + m_startTime;
	}

	if ( elapsedTime >= m_duration )
	{
		return true;
	}

	return false;
}


//-----------------------------------------------------------------------------------------------
bool Stopwatch::CheckAndDecrement()
{
	if ( HasElapsed() )
	{
		m_startTime += m_duration;
		return true;
	}

	return false;
}


//-----------------------------------------------------------------------------------------------
bool Stopwatch::CheckAndRestart()
{
	if ( HasElapsed() )
	{
		m_startTime = m_sourceClock->GetTotalSeconds();
		return true;
	}

	return false;
}


//-----------------------------------------------------------------------------------------------
int Stopwatch::DecrementAll()
{
	int count = 0;

	while ( CheckAndDecrement() )
		count++;

	return count;
}


//-----------------------------------------------------------------------------------------------
void Stopwatch::Pause()
{
	m_isPaused = true;
	m_pausedDelta = m_sourceClock->GetTotalSeconds() - m_startTime;
}


//-----------------------------------------------------------------------------------------------
void Stopwatch::Resume()
{
	m_startTime = m_sourceClock->GetTotalSeconds() - m_pausedDelta;
	m_isPaused = false;
}


//-----------------------------------------------------------------------------------------------
bool Stopwatch::IsPaused() const
{
	return m_isPaused;
}


