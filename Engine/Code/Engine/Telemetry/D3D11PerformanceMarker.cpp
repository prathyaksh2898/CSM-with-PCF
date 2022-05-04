#include "D3D11PerformanceMarker.hpp"


#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/StringUtils.hpp"


//--------------------------------------------------------------------------------------------------------------------------------------------
//
//--------------------------------------------------------------------------------------------------------------------------------------------
//                PROFILING D3D POINTER FOR THE SPECIFIC CONFIGURATIONS
//--------------------------------------------------------------------------------------------------------------------------------------------

#include <guiddef.h>
#include <cguid.h>
#include <atlbase.h>
#include <d3d11_1.h>

CComPtr<ID3DUserDefinedAnnotation> pPerfMarker;
D3D11PerformanceMarker* g_theD3D11PerfMarker = nullptr;


//--------------------------------------------------------------------------------------------------------------------------------------------
D3D11PerformanceMarker::D3D11PerformanceMarker( Renderer* renderContext )
{
	HRESULT objectCreationSuccessful = renderContext->GetDeviceContext()->QueryInterface( __uuidof( pPerfMarker ), reinterpret_cast< void** >( &pPerfMarker ) );

	if ( FAILED( objectCreationSuccessful ) )
	{
		__debugbreak();
	}
}


//--------------------------------------------------------------------------------------------------------------------------------------------
D3D11PerformanceMarker::~D3D11PerformanceMarker()
{

}


//--------------------------------------------------------------------------------------------------------------------------------------------
void D3D11PerformanceMarker::BeginPerformanceMarker( LPCWSTR name )
{
	pPerfMarker->BeginEvent( name );
}


//--------------------------------------------------------------------------------------------------------------------------------------------
void D3D11PerformanceMarker::EndPerformanceMarker()
{
	pPerfMarker->EndEvent();
}


//--------------------------------------------------------------------------------------------------------------------------------------------
ZoneScopedD3D11Marker::ZoneScopedD3D11Marker( const char* name )
{
	std::string nameAsString = name;
	g_theD3D11PerfMarker->BeginPerformanceMarker( StringToWideString( nameAsString ).c_str() );
}


//--------------------------------------------------------------------------------------------------------------------------------------------
ZoneScopedD3D11Marker::ZoneScopedD3D11Marker( const wchar_t* name )
{
	g_theD3D11PerfMarker->BeginPerformanceMarker( name );
}


//--------------------------------------------------------------------------------------------------------------------------------------------
ZoneScopedD3D11Marker::~ZoneScopedD3D11Marker()
{
	g_theD3D11PerfMarker->EndPerformanceMarker();
}