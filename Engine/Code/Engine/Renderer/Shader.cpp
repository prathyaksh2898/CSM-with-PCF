#include "Shader.hpp"

#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/D3D11Internal.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/VertexData/Vertex_PCU.hpp"
#include "Engine/Renderer/VertexData/Vertex_PCUTBN.hpp"
#include "Engine/Renderer/ErrorShaderSource.hpp"

#include <d3dcompiler.h>
#pragma comment (lib, "d3dcompiler.lib")


//------------------------------------------------------------------------------------------------
bool Shader::IsValid() const
{
	return true;
}


//------------------------------------------------------------------------------------------------
std::string const& Shader::GetName() const
{
	return m_config.m_name;
}


//------------------------------------------------------------------------------------------------
bool Shader::RecompileShader()
{
	Destroy();
	return Create( m_sourceRenderer, m_config );
}


//------------------------------------------------------------------------------------------------
ID3D11InputLayout* Shader::CreateOrGetInputLayoutFor_Vertex_PCU()
{
	if ( m_inputLayoutFor_Vertex_PCU != nullptr )
	{
		return m_inputLayoutFor_Vertex_PCU;
	}

	D3D11_INPUT_ELEMENT_DESC vertexDesc[ 3 ];

	vertexDesc[ 0 ].SemanticName = "POSITION";
	vertexDesc[ 0 ].SemanticIndex = 0;
	vertexDesc[ 0 ].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[ 0 ].InputSlot = 0;
	vertexDesc[ 0 ].AlignedByteOffset = offsetof( Vertex_PCU, m_position );
	vertexDesc[ 0 ].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	vertexDesc[ 0 ].InstanceDataStepRate = 0;

	vertexDesc[ 1 ].SemanticName = "COLOR";
	vertexDesc[ 1 ].SemanticIndex = 0;
	vertexDesc[ 1 ].Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	vertexDesc[ 1 ].InputSlot = 0;
	vertexDesc[ 1 ].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	vertexDesc[ 1 ].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	vertexDesc[ 1 ].InstanceDataStepRate = 0;

	vertexDesc[ 2 ].SemanticName = "TEXCOORD";
	vertexDesc[ 2 ].SemanticIndex = 0;
	vertexDesc[ 2 ].Format = DXGI_FORMAT_R32G32_FLOAT;
	vertexDesc[ 2 ].InputSlot = 0;
	vertexDesc[ 2 ].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	vertexDesc[ 2 ].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	vertexDesc[ 2 ].InstanceDataStepRate = 0;

	ASSERT_OR_DIE( m_vertexByteCode.size() != 0, "Vertex Byte Code is Empty!" );

	m_sourceRenderer->GetDevice()->CreateInputLayout(
		vertexDesc,
		3,
		&m_vertexByteCode[ 0 ],
		m_vertexByteCode.size(),
		&m_inputLayoutFor_Vertex_PCU
	);

	ASSERT_OR_DIE( m_inputLayoutFor_Vertex_PCU != nullptr, "Failure in creating Input Layout" );

	return m_inputLayoutFor_Vertex_PCU;
}


//------------------------------------------------------------------------------------------------
ID3D11InputLayout* Shader::CreateOrGetInputLayoutFor_Vertex_PCUTBN()
{
	if ( m_inputLayoutFor_Vertex_PCUTBN != nullptr )
	{
		return m_inputLayoutFor_Vertex_PCUTBN;
	}

	D3D11_INPUT_ELEMENT_DESC vertexDesc[ 6 ];

	vertexDesc[ 0 ].SemanticName = "POSITION";
	vertexDesc[ 0 ].SemanticIndex = 0;
	vertexDesc[ 0 ].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[ 0 ].InputSlot = 0;
	vertexDesc[ 0 ].AlignedByteOffset = offsetof( Vertex_PCUTBN, m_position );
	vertexDesc[ 0 ].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	vertexDesc[ 0 ].InstanceDataStepRate = 0;

	vertexDesc[ 1 ].SemanticName = "COLOR";
	vertexDesc[ 1 ].SemanticIndex = 0;
	vertexDesc[ 1 ].Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	vertexDesc[ 1 ].InputSlot = 0;
	vertexDesc[ 1 ].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	vertexDesc[ 1 ].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	vertexDesc[ 1 ].InstanceDataStepRate = 0;

	vertexDesc[ 2 ].SemanticName = "TEXCOORD";
	vertexDesc[ 2 ].SemanticIndex = 0;
	vertexDesc[ 2 ].Format = DXGI_FORMAT_R32G32_FLOAT;
	vertexDesc[ 2 ].InputSlot = 0;
	vertexDesc[ 2 ].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	vertexDesc[ 2 ].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	vertexDesc[ 2 ].InstanceDataStepRate = 0;

	vertexDesc[ 3 ].SemanticName = "TANGENT";
	vertexDesc[ 3 ].SemanticIndex = 0;
	vertexDesc[ 3 ].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[ 3 ].InputSlot = 0;
	vertexDesc[ 3 ].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	vertexDesc[ 3 ].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	vertexDesc[ 3 ].InstanceDataStepRate = 0;

	vertexDesc[ 4 ].SemanticName = "BINORMAL";
	vertexDesc[ 4 ].SemanticIndex = 0;
	vertexDesc[ 4 ].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[ 4 ].InputSlot = 0;
	vertexDesc[ 4 ].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	vertexDesc[ 4 ].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	vertexDesc[ 4 ].InstanceDataStepRate = 0;

	vertexDesc[ 5 ].SemanticName = "NORMAL";
	vertexDesc[ 5 ].SemanticIndex = 0;
	vertexDesc[ 5 ].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[ 5 ].InputSlot = 0;
	vertexDesc[ 5 ].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	vertexDesc[ 5 ].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	vertexDesc[ 5 ].InstanceDataStepRate = 0;

	ASSERT_OR_DIE( m_vertexByteCode.size() != 0, "Vertex Byte Code is Empty!" );

	m_sourceRenderer->GetDevice()->CreateInputLayout(
		vertexDesc,
		_countof( vertexDesc ),
		&m_vertexByteCode[ 0 ],
		m_vertexByteCode.size(),
		&m_inputLayoutFor_Vertex_PCUTBN
	);

	ASSERT_OR_DIE( m_inputLayoutFor_Vertex_PCUTBN != nullptr, "Failure in creating Input Layout" );

	return m_inputLayoutFor_Vertex_PCUTBN;
}


//------------------------------------------------------------------------------------------------
Shader::Shader()
{
	m_config.m_name = "Data/Shader/Default";
}


//------------------------------------------------------------------------------------------------
Shader::~Shader()
{
}


//------------------------------------------------------------------------------------------------
bool Shader::Create( Renderer* renderer, const ShaderConfig& config, std::string source /*= ""*/ )
{
	m_config = config;
	m_sourceRenderer = renderer;

	std::string shaderName = m_config.m_name;
	std::string shaderSource;

	if ( source.length() > 0 )
	{
		shaderSource = source;
	}
	else if ( !FileReadToString( shaderSource, shaderName.append( ".hlsl" ).c_str() ) )
	{
		ERROR_AND_DIE( "Error - Reading the shader " + m_config.m_name );
	}

	bool geometryShaderCompiles = CompileGeometryShader( renderer, shaderSource );
	bool pixelShaderCompiles    = CompilePixelShader( renderer, shaderSource );
	bool vertexShaderCompiles   = CompileVertexShader( renderer, shaderSource );

	if ( !geometryShaderCompiles && !pixelShaderCompiles && !vertexShaderCompiles )
	{
		DebuggerPrintf( Stringf( "WARNING: SHADER FAILED TO COMPILE - %s\n", shaderName.c_str() ).c_str() );
		Create( renderer, config, g_errorShaderSource );
		//ERROR_AND_DIE( "Error - Vertex/Pixel Shader Compile Failed" );
		return false;
	}

	return true;
}


//------------------------------------------------------------------------------------------------
void Shader::Destroy()
{
	DX_SAFE_RELEASE( m_inputLayoutFor_Vertex_PCU );
	DX_SAFE_RELEASE( m_inputLayoutFor_Vertex_PCUTBN );
	DX_SAFE_RELEASE( m_vertexShader );
	DX_SAFE_RELEASE( m_pixelShader );
}


//------------------------------------------------------------------------------------------------
bool Shader::CompileVertexShader( Renderer* renderer, std::string shaderSource )
{
	DWORD compileFlags = 0;

#if defined(ENGINE_DEBUG_RENDERER)

	compileFlags |= D3DCOMPILE_DEBUG;
	compileFlags |= D3DCOMPILE_WARNINGS_ARE_ERRORS;

#else

	compileFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;

#endif

	bool success;
	ID3DBlob* vertexByteCode = nullptr;
	ID3DBlob* vertexShaderErrorBuffer = nullptr;

	HRESULT hResult = D3DCompile(
		shaderSource.c_str(),
		shaderSource.size(),
		m_config.m_name.c_str(),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		m_config.m_vertexEntryPoint.c_str(),
		"vs_5_0",
		compileFlags,
		0,
		&vertexByteCode,
		&vertexShaderErrorBuffer
	);

	if ( SUCCEEDED( hResult ) && vertexByteCode != nullptr )
	{
		renderer->GetDevice()->CreateVertexShader(
			vertexByteCode->GetBufferPointer(),
			vertexByteCode->GetBufferSize(),
			nullptr,
			&m_vertexShader
		);

		success = true;

		m_vertexByteCode.resize( vertexByteCode->GetBufferSize() );
		memcpy( m_vertexByteCode.data(), reinterpret_cast< uint8_t* >( vertexByteCode->GetBufferPointer() ), vertexByteCode->GetBufferSize() );

	}
	else
	{
		char const* errorString = "";

		if ( vertexShaderErrorBuffer != nullptr )
		{
			errorString = reinterpret_cast< char const* >( vertexShaderErrorBuffer->GetBufferPointer() );
		}

		DebuggerPrintf( errorString );
		success = false;
	}

	DX_SAFE_RELEASE( vertexByteCode );
	DX_SAFE_RELEASE( vertexShaderErrorBuffer );

	return success;
}


//------------------------------------------------------------------------------------------------
bool Shader::CompilePixelShader( Renderer* renderer, std::string shaderSource )
{
	DWORD compileFlags = 0;

#if defined(ENGINE_DEBUG_RENDERER)

	compileFlags |= D3DCOMPILE_DEBUG;
	compileFlags |= D3DCOMPILE_WARNINGS_ARE_ERRORS;

#else

	compileFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;

#endif

	bool success;
	ID3DBlob* pixelByteCode = nullptr;
	ID3DBlob* pixelShaderErrorBuffer = nullptr;

	HRESULT hResult = D3DCompile(
		shaderSource.c_str(),
		shaderSource.size(),
		m_config.m_name.c_str(),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		m_config.m_pixelEntryPoint.c_str(),
		"ps_5_0",
		compileFlags,
		0,
		&pixelByteCode,
		&pixelShaderErrorBuffer
	);

	if ( SUCCEEDED( hResult ) && pixelByteCode != nullptr )
	{
		renderer->GetDevice()->CreatePixelShader(
			pixelByteCode->GetBufferPointer(),
			pixelByteCode->GetBufferSize(),
			nullptr,
			&m_pixelShader
		);

		success = true;
	}
	else
	{
		char const* errorString = "";

		if ( pixelShaderErrorBuffer != nullptr )
		{
			errorString = reinterpret_cast< char const* >( pixelShaderErrorBuffer->GetBufferPointer() );
		}

		DebuggerPrintf( errorString );

		success = false;
	}

	DX_SAFE_RELEASE( pixelByteCode );
	DX_SAFE_RELEASE( pixelShaderErrorBuffer );

	return success;
}


//------------------------------------------------------------------------------------------------
bool Shader::CompileGeometryShader( Renderer* renderer, std::string shaderSource )
{
	DWORD compileFlags = 0;

#if defined(ENGINE_DEBUG_RENDERER)

	compileFlags |= D3DCOMPILE_DEBUG;
	compileFlags |= D3DCOMPILE_WARNINGS_ARE_ERRORS;

#else

	compileFlags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;

#endif

	bool success;
	ID3DBlob* geometryByteCode = nullptr;
	ID3DBlob* geometryShaderErrorBuffer = nullptr;

	HRESULT hResult = D3DCompile(
		shaderSource.c_str(),
		shaderSource.size(),
		m_config.m_name.c_str(),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		m_config.m_geometryEntryPoint.c_str(),
		"gs_5_0",
		compileFlags,
		0,
		&geometryByteCode,
		&geometryShaderErrorBuffer
	);

	if ( SUCCEEDED( hResult ) && geometryByteCode != nullptr )
	{
		renderer->GetDevice()->CreateGeometryShader(
			geometryByteCode->GetBufferPointer(),
			geometryByteCode->GetBufferSize(),
			nullptr,
			&m_geometryShader
		);

		success = true;
	}
	else
	{
		char const* errorString = "";

		if ( geometryShaderErrorBuffer != nullptr )
		{
			errorString = reinterpret_cast< char const* >( geometryShaderErrorBuffer->GetBufferPointer() );
		}

		DebuggerPrintf( errorString );

		success = false;
	}

	DX_SAFE_RELEASE( geometryByteCode );
	DX_SAFE_RELEASE( geometryShaderErrorBuffer );

	return success;
}
