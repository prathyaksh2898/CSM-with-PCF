#pragma once


//-----------------------------------------------------------------------------------------------
class Clock;


//-----------------------------------------------------------------------------------------------
class Stopwatch
{
public:
	Stopwatch();
	explicit Stopwatch( double secondDuration );
	Stopwatch( Clock const& sourceClock, double secondDuration );

	void   Start( Clock const& sourceClock, double seconds );
	void   Start( double seconds );
	void   Start();
	void   SetClock( Clock const& sourceClock ); // changes clock, but keeps current elapsed time; 
	void   Restart();
	void   Stop();

	double GetDuration() const { return m_duration; }
	double GetElapsedSeconds() const;
	float  GetElapsedFraction() const;
	bool   IsStopped() const;

	// Queries
	bool   HasElapsed() const;
	bool   Check() const { return HasElapsed(); }
	bool   CheckAndDecrement();
	bool   CheckAndRestart();
	int    DecrementAll();

	void   Pause();
	void   Resume();
	bool   IsPaused() const;

private:
	Clock const* m_sourceClock = nullptr;
	double       m_startTime   = 0.0;
	double       m_duration    = 0.0;
	double       m_pausedDelta = 0.0;

	bool         m_isStopped   = false;
	bool         m_isPaused    = false;
};
