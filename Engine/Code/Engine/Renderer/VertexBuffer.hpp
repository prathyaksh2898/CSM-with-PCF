#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/VertexData/Vertex_PCU.hpp"
#include "Engine/Renderer/VertexData/Vertex_PCUTBN.hpp"


//------------------------------------------------------------------------------------------------
struct ID3D11Buffer;
class Renderer;


//------------------------------------------------------------------------------------------------
class VertexBuffer
{
	friend class Renderer;

public:
	void CopyVertexData( void const* data, size_t byteCount, size_t byteSize = sizeof( Vertex_PCU ) );

	inline size_t GetStride() const { return m_byteSize; }

protected:
	VertexBuffer( Renderer* source, size_t const initialSize = 0 );
	VertexBuffer( VertexBuffer const& copy ) = delete;
	virtual ~VertexBuffer();

	ID3D11Buffer* GetHandle() const;

protected:
	Renderer*     m_sourceRenderer  = nullptr;	
	ID3D11Buffer* m_gpuBuffer       = nullptr;	
							        
	size_t        m_byteSize        = 0;    
	size_t        m_byteMaxSize     = 0;
	
};


