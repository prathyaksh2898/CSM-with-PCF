#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/EngineCommon.hpp"

#if defined(_DIRECTX11)
#include "Engine/Renderer/D3D11Internal.hpp"


//------------------------------------------------------------------------------------------------
void VertexBuffer::CopyVertexData( void const* data, size_t byteCount, size_t byteSize /*= sizeof( Vertex_PCU )*/ )
{
	m_byteSize = byteSize;

	if (m_byteMaxSize < byteCount)
	{
		DX_SAFE_RELEASE( m_gpuBuffer );
		D3D11_BUFFER_DESC bufferDesc;
		bufferDesc.ByteWidth = static_cast<UINT>(byteCount);
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;

		m_sourceRenderer->GetDevice()->CreateBuffer( &bufferDesc, nullptr, &m_gpuBuffer );
		ASSERT_OR_DIE( m_gpuBuffer != nullptr, "Failed to create Vertex Buffer." );

		m_byteMaxSize = byteCount;
	}

	D3D11_MAPPED_SUBRESOURCE subResourceMapping;

	HRESULT hResult = m_sourceRenderer->GetDeviceContext()->Map(
		m_gpuBuffer,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&subResourceMapping
	);

	ASSERT_OR_DIE( SUCCEEDED( hResult ), "Failed to map buffer for write." );

	memcpy( subResourceMapping.pData, data, byteCount );

	m_sourceRenderer->GetDeviceContext()->Unmap( m_gpuBuffer, 0 );

}


//------------------------------------------------------------------------------------------------
VertexBuffer::VertexBuffer( Renderer* source, size_t const initialSize /*= 0 */ )
{
	m_sourceRenderer = source;
	m_byteMaxSize = initialSize;
}


//------------------------------------------------------------------------------------------------
VertexBuffer::~VertexBuffer()
{
	m_sourceRenderer = nullptr;
	DX_SAFE_RELEASE( m_gpuBuffer );
}


//------------------------------------------------------------------------------------------------
ID3D11Buffer* VertexBuffer::GetHandle() const
{
	return m_gpuBuffer;
}

#endif

