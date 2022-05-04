#pragma once

#include "Engine/Core/EngineCommon.hpp"

#if defined(_DIRECTX11)
/** includes you need **/
// ...

// predefines
struct ID3D11Buffer;
class Renderer;

//------------------------------------------------------------------------------------------------
class ConstantBuffer
{
	friend class Renderer; // Only the Renderer can create new Texture objects!

public:
	// updates data in the buffer, byteCount must be less than or equal to constant buffer size; 
	bool SetData(void const* data, size_t byteCount);

	// templated helper
	template <typename STRUCT_TYPE>
	bool SetData(STRUCT_TYPE const& data)
	{
		return SetData(&data, sizeof(STRUCT_TYPE));
	}

protected:
	ConstantBuffer(Renderer* source, size_t const maxSize); // can't instantiate directly; must ask Renderer to do it for you
	ConstantBuffer(ConstantBuffer const& copy) = delete; // No copying allowed!  This represents GPU memory.
	virtual ~ConstantBuffer();

	ID3D11Buffer* GetHandle();


protected:
	ID3D11Buffer* m_gpuBuffer = nullptr;

	size_t m_size = 0; // last number of bytes read
	size_t m_maxSize = 0; // max size of this buffer

	Renderer* m_sourceRenderer = nullptr;
};

#endif
