#pragma once

typedef const wchar_t* LPCWSTR;

//--------------------------------------------------------------------------------------------------------------------------------------------
class Renderer;


//--------------------------------------------------------------------------------------------------------------------------------------------
class D3D11PerformanceMarker
{
public:

	// Will only be created when special Profilling configuration are created and pre-processor are added to the configurations in project settings
	D3D11PerformanceMarker( Renderer* renderContext );
	~D3D11PerformanceMarker();

	void BeginPerformanceMarker( LPCWSTR name );
	void EndPerformanceMarker();
};


//--------------------------------------------------------------------------------------------------------------------------------------------
struct ZoneScopedD3D11Marker
{
public:
	ZoneScopedD3D11Marker( const char* name );
	ZoneScopedD3D11Marker( const wchar_t* name );
	~ZoneScopedD3D11Marker();
};
