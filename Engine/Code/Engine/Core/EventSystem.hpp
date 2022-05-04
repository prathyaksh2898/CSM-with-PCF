#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include <vector>
#include <mutex>
#include <map>
#include <string>


//------------------------------------------------------------------------------------------------
struct EventSystemConfig
{

};


//------------------------------------------------------------------------------------------------
class NamedStrings;
typedef NamedStrings EventArgs;
typedef bool (*EventCallbackFunction)(EventArgs& args);


//------------------------------------------------------------------------------------------------
struct EventSubscription
{
	int m_numTimesTriggered = 0;
	EventCallbackFunction m_callbackFunctionPtr = nullptr;
};


//------------------------------------------------------------------------------------------------
typedef std::vector<EventSubscription> SubscriptionList;


//------------------------------------------------------------------------------------------------
class EventSystem 
{

public:
	EventSystem(EventSystemConfig const& config);
	~EventSystem();
	void Startup();
	void Shutdown();
	void BeginFrame();
	void EndFrame();

	void SubscribeEventCallbackFunction(std::string const& eventName, EventCallbackFunction functionPtr);
	void UnsubscribeEventCallbackFunction(std::string const& eventName, EventCallbackFunction functionPtr);
	void FireEvent(std::string const& eventName, EventArgs& args);
	void FireEvent(std::string const& eventName);

	static bool ListValidEventsForHelp( EventArgs& args );

	bool IsValidEvent( std::string eventName );

protected:
	EventSystemConfig                       m_config;
	std::map<std::string, SubscriptionList> m_subscriptionListsByEventName;
	std::mutex                              m_eventSystemLock;

};


//------------------------------------------------------------------------------------------------
void SubscribeEventCallbackFunction(std::string const& eventName, EventCallbackFunction functionPtr);
void UnsubscribeEventCallbackFunction(std::string const& eventName, EventCallbackFunction functionPtr);
void FireEvent(std::string const& eventName, EventArgs& args);
void FireEvent(std::string const& eventName);