#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include <string>
#include <vector>


//------------------------------------------------------------------------------------------------
// predefines so this compiles
struct ID3D11InputLayout;
struct ID3D11GeometryShader;
struct ID3D11PixelShader;
struct ID3D11VertexShader;

class Renderer;


//------------------------------------------------------------------------------------------------
struct ShaderConfig
{
	std::string m_name;								    // for now, file path without an extension 
													    
	std::string m_vertexEntryPoint   = "VertexMain";    // which function in the source file we use as the entry point for the vertex stage
	std::string m_pixelEntryPoint    = "PixelMain";	    // which function in the source file we use as the entry point for the pixel stage
	std::string m_geometryEntryPoint = "GeometryMain";	// which function in the source file we use as the entry point for the geometry stage
};


//------------------------------------------------------------------------------------------------
class Shader
{
	friend class Renderer;

public:
	bool               IsValid() const;
	std::string const& GetName() const;

	bool RecompileShader();

protected:
	ID3D11InputLayout* CreateOrGetInputLayoutFor_Vertex_PCU();
	ID3D11InputLayout* CreateOrGetInputLayoutFor_Vertex_PCUTBN();

private:
	Shader();
	Shader( Shader const& copy ) = delete;
	~Shader();

	bool Create( Renderer* renderer, const ShaderConfig& config, std::string source = "" );
	void Destroy();

	bool CompileVertexShader( Renderer* renderer, std::string shaderSource );
	bool CompilePixelShader( Renderer* renderer, std::string shaderSource );
	bool CompileGeometryShader( Renderer* renderer, std::string shaderSource );

protected:
	ShaderConfig          m_config;
	Renderer*             m_sourceRenderer               = nullptr;
						 				                
	ID3D11VertexShader*   m_vertexShader                 = nullptr;
	ID3D11PixelShader*    m_pixelShader                  = nullptr;
	ID3D11GeometryShader* m_geometryShader               = nullptr;
						 
	ID3D11InputLayout*    m_inputLayoutFor_Vertex_PCU    = nullptr;
	ID3D11InputLayout*    m_inputLayoutFor_Vertex_PCUTBN = nullptr;
						  
	std::vector<uint8_t>  m_vertexByteCode;
};

