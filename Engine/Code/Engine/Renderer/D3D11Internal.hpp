#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <d3d11.h>
#include <dxgi.h>

#define DX_SAFE_RELEASE(obj) if ((obj)!= nullptr) { (obj)->Release(); (obj) = nullptr; }
#undef OPAQUE