#pragma once
#include <vector>


//----------------------------------------------------------------------------------------------------
class Clock
{
	friend class Clock;

public:
	Clock();
	explicit Clock( Clock* parent );
	~Clock();

	Clock( Clock const& copy ) = delete;

	void          SetParent( Clock& parent );
		          
	void          Pause();
	void          Unpause();
	void          TogglePause();
	void          StepFrame(); 
	void          SetTimeDilation( double dilationAmount );
	double        GetTimeDilation() const { return m_timeDilation; }
		          
	void          Reset( bool resetChildren = true );
		          
	double        GetFrameDeltaSeconds() const { return m_frameDeltaSeconds; }
	double        GetTotalSeconds() const { return m_totalSecondsPassed; }
	size_t        GetFrameCount() const { return m_frameCount; }
		          
	bool          IsPaused() const { return m_isPaused; }

	void          RemoveSelfFromParent();

public:
	static void   SystemBeginFrame();
	static Clock& GetSystemClock();


protected: 

	void          Tick();
		          
	void          AdvanceTime( double deltaTimeSeconds );
		          
	void          AddChild( Clock* childClock );
	void          RemoveChild( Clock* childClock );

protected: 
	Clock*              m_parent             = nullptr;
	std::vector<Clock*> m_children;
	
	double              m_totalSecondsPassed = 0.0;	
	double              m_frameDeltaSeconds  = 0.0;	
	size_t              m_frameCount         = 0;	
		                
	double              m_timeDilation       = 1.0;
	bool                m_isPaused           = false;
	bool                m_pauseAfterFrame    = false;
		                				     
	double              m_lastUpdateTime     = 0.0;
};




