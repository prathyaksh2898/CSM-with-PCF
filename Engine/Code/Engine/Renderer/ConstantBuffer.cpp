#include "Engine/Renderer/ConstantBuffer.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

#if defined(_DIRECTX11)

#include "Engine/Renderer/D3D11Internal.hpp"

//------------------------------------------------------------------------------------------------
bool ConstantBuffer::SetData(void const* data, size_t byteCount)
{
	D3D11_MAPPED_SUBRESOURCE subResourceMapping;

	HRESULT hResult = m_sourceRenderer->GetDeviceContext()->Map(
		m_gpuBuffer,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&subResourceMapping
	);

	ASSERT_OR_DIE(SUCCEEDED(hResult), "Failed to map buffer for write.");

	memcpy(subResourceMapping.pData, data, byteCount);

	m_sourceRenderer->GetDeviceContext()->Unmap(m_gpuBuffer, 0);

	m_size = byteCount;

	return true;
}

 
//------------------------------------------------------------------------------------------------
ConstantBuffer::ConstantBuffer(Renderer* source, size_t const maxSize)
{
	m_sourceRenderer = source;

	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.ByteWidth = static_cast<UINT>(maxSize);
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	m_sourceRenderer->GetDevice()->CreateBuffer(&bufferDesc, nullptr, &m_gpuBuffer);
	ASSERT_OR_DIE(m_gpuBuffer != nullptr, "Failed to create Constant Buffer.");

	m_maxSize = maxSize;
}


//------------------------------------------------------------------------------------------------
ConstantBuffer::~ConstantBuffer()
{
	DX_SAFE_RELEASE(m_gpuBuffer);
	m_sourceRenderer = nullptr;
}


//------------------------------------------------------------------------------------------------
ID3D11Buffer* ConstantBuffer::GetHandle()
{
	return m_gpuBuffer;
}


#endif

