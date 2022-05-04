#pragma once
#include "App.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/EngineCommon.hpp"

//-----------------------------------------------------------------------------------------------
class App;
class AudioSystem;
class D3D11PerformanceMarker;
class DebugUISystem;
class Game;
class InputSystem;
class RandomNumberGenerator;
class Renderer;
class Window;

struct Rgba8;


//-----------------------------------------------------------------------------------------------
constexpr float CLIENT_ASPECT = 2.0f;
constexpr float WORLD_SIZE_X = 200.f;
constexpr float WORLD_SIZE_Y = 100.f;
constexpr float SCREEN_SIZE_X = 1600.f;
constexpr float SCREEN_SIZE_Y = 800.f;
constexpr float SCREEN_CENTER_Y = SCREEN_SIZE_Y * 0.5f;
constexpr float SCREEN_CENTER_X = SCREEN_SIZE_X * 0.5f;
constexpr float WORLD_CENTER_X = WORLD_SIZE_X / 2.f;
constexpr float WORLD_CENTER_Y = WORLD_SIZE_Y / 2.f;


//-----------------------------------------------------------------------------------------------
extern App*                    g_theApp;
extern AudioSystem*            g_theAudio;
extern D3D11PerformanceMarker* g_theD3D11PerfMarker;
extern DebugUISystem*          g_theDebugUISystem;
extern Game*                   g_theGame;
extern InputSystem*            g_theInput;
extern RandomNumberGenerator   g_rng;
extern Renderer*               g_theRenderer;
extern Window*                 g_theWindow;

