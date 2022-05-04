#pragma once
#include "NamedStrings.hpp"
#include "Game/EngineBuildPreferences.hpp"
#define UNUSED(x) (void)(x);


//------------------------------------------------------------------------------------------------
typedef unsigned int uint;

//------------------------------------------------------------------------------------------------
class DevConsole;
class EventSystem;
class FBXLoader;
class Renderer;
class VisualDatabase;


//------------------------------------------------------------------------------------------------
extern DevConsole*     g_theConsole;
extern EventSystem*    g_theEventSystem;
extern FBXLoader*      g_theFBXLoader;
extern NamedStrings    g_gameConfigBlackboard;
extern Renderer*       g_theRenderer;
extern VisualDatabase* g_theVisualDatabase;


//------------------------------------------------------------------------------------------------
