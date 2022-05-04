#include "EventSystem.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/DevConsole.hpp"


//------------------------------------------------------------------------------------------------
EventSystem* g_theEventSystem = nullptr;


//------------------------------------------------------------------------------------------------
EventSystem::EventSystem(EventSystemConfig const& config):
	m_config(config)
{

}


//------------------------------------------------------------------------------------------------
EventSystem::~EventSystem()
{

}


//------------------------------------------------------------------------------------------------
void EventSystem::Startup()
{
	SubscribeEventCallbackFunction( "Help", EventSystem::ListValidEventsForHelp );
}


//------------------------------------------------------------------------------------------------
void EventSystem::Shutdown()
{

}


//------------------------------------------------------------------------------------------------
void EventSystem::BeginFrame()
{

}


//------------------------------------------------------------------------------------------------
void EventSystem::EndFrame()
{

}


//------------------------------------------------------------------------------------------------
void EventSystem::SubscribeEventCallbackFunction(std::string const& eventName, EventCallbackFunction functionPtr)
{
	std::vector<EventSubscription>& subscriberListForThisEvent = m_subscriptionListsByEventName[eventName];
	
	EventSubscription newSubscription;
	newSubscription.m_callbackFunctionPtr = functionPtr;
	m_eventSystemLock.lock();
	subscriberListForThisEvent.push_back(newSubscription);
	m_eventSystemLock.unlock();
}


//------------------------------------------------------------------------------------------------
void EventSystem::UnsubscribeEventCallbackFunction(std::string const& eventName, EventCallbackFunction functionPtr)
{
	m_eventSystemLock.lock();
	std::vector<EventSubscription>& subscriberListForThisEvent = m_subscriptionListsByEventName[eventName];
	int numSubscriptions = static_cast<int>(subscriberListForThisEvent.size());
	bool isUnsubscribed = false;

	for (int subscriptionNum = 0; subscriptionNum < numSubscriptions; subscriptionNum++) 
	{
		if (subscriberListForThisEvent[subscriptionNum].m_callbackFunctionPtr == functionPtr) 
		{
			subscriberListForThisEvent.erase(subscriberListForThisEvent.begin() + subscriptionNum);
			isUnsubscribed = true;
		}
	}	
	m_eventSystemLock.unlock();

	GUARANTEE_RECOVERABLE(isUnsubscribed, "Function Pointer not in Subscription");
}


//------------------------------------------------------------------------------------------------
void EventSystem::FireEvent(std::string const& eventName, EventArgs& args)
{
	m_eventSystemLock.lock();
	std::vector<EventSubscription>& subscriberListForThisEvent = m_subscriptionListsByEventName[eventName];
	int numSubscriptions = static_cast<int>(subscriberListForThisEvent.size());

	if (numSubscriptions == 0)
	{
		if(g_theConsole != nullptr)
			g_theConsole->AddLine(DevConsole::ERROR, Stringf("Invalid Command: %s",eventName.c_str()));
	}

	for (int subscriptionNum = 0; subscriptionNum < numSubscriptions; subscriptionNum++) 
	{
		EventSubscription& subscription = subscriberListForThisEvent[subscriptionNum];
		if (subscription.m_callbackFunctionPtr) 
		{
			subscription.m_numTimesTriggered++;
			
			bool wasConsumed = subscription.m_callbackFunctionPtr(args);
			if (wasConsumed) 
			{
				break;
			}
		}
	}
	m_eventSystemLock.unlock();
}


//------------------------------------------------------------------------------------------------
void EventSystem::FireEvent(std::string const& eventName)
{
	EventArgs args;
	FireEvent(eventName, args);
}


//------------------------------------------------------------------------------------------------
bool EventSystem::ListValidEventsForHelp( EventArgs& args )
{
	std::string filter = args.GetValue( "filter", "" );
	
	std::map<std::string, SubscriptionList>::iterator it;

	g_theConsole->AddLine( DevConsole::INFO_MINOR, "List of Available Commands:" );

	for (it = g_theEventSystem->m_subscriptionListsByEventName.begin(); it != g_theEventSystem->m_subscriptionListsByEventName.end(); it++)
	{
		if (filter == "")
		{
			if (it->second.size() > 0) 
			{
				g_theConsole->AddLine( DevConsole::INFO_MINOR, it->first );
			}
		}

		else
		{
			if (it->first.find( filter ) != std::string::npos)
			{
				if (it->second.size() > 0)
				{
					g_theConsole->AddLine( DevConsole::INFO_MINOR, it->first );
				}
			}
		}

	}

	return true;
}


//------------------------------------------------------------------------------------------------
bool EventSystem::IsValidEvent( std::string eventName )
{
	std::map<std::string, SubscriptionList>::iterator it;
	for (it = m_subscriptionListsByEventName.begin(); it != m_subscriptionListsByEventName.end(); it++)
	{
		if (it->first == eventName && it->second.size() > 0)
		{
			return true;
		}
	}

	return false;
}


//------------------------------------------------------------------------------------------------
void SubscribeEventCallbackFunction(std::string const& eventName, EventCallbackFunction functionPtr)
{
	if (g_theEventSystem) 
	{
		g_theEventSystem->SubscribeEventCallbackFunction(eventName, functionPtr);
	}
}


//------------------------------------------------------------------------------------------------
void UnsubscribeEventCallbackFunction(std::string const& eventName, EventCallbackFunction functionPtr)
{
	if (g_theEventSystem) 
	{
		g_theEventSystem->UnsubscribeEventCallbackFunction(eventName, functionPtr);
	}
}


//------------------------------------------------------------------------------------------------
void FireEvent(std::string const& eventName, EventArgs& args)
{
	if (g_theEventSystem) 
	{
		g_theEventSystem->FireEvent(eventName, args);
	}
}


//------------------------------------------------------------------------------------------------
void FireEvent(std::string const& eventName)
{
	if (g_theEventSystem) 
	{
		g_theEventSystem->FireEvent(eventName);
	}
}
