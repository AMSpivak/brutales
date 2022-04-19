#ifndef CONSTANT_BUFFER_H
#define CONSTANT_BUFFER_H

#include "PlatformDefine.h"
#include "Helpers.h"

namespace BruteForce
{
	template <class BufferStruct>
	class ConstantBuffer
	{
	private:
		
	public:
		Resource m_GpuBuffer;
		BufferStruct* m_CpuBuffer;
		UINT8* m_GpuAddress;

		ConstantBuffer()
		{
			m_GpuAddress = nullptr;
			m_CpuBuffer = new BufferStruct();
			ZeroMemory(m_CpuBuffer, sizeof(BufferStruct));
		}

		~ConstantBuffer()
		{
			delete m_CpuBuffer;
		}

		size_t GetResourceHeapSize()
		{
			constexpr size_t minbuffer_size = 1024 * 64 - 1; // size of the resource heap. Must be a multiple of 64KB
			return (sizeof(BufferStruct) + minbuffer_size) & ~minbuffer_size;
		}

		size_t GetBufferSize()
		{
			constexpr size_t minbuffer_size = 255; // size of the resource heap. Must be a multiple of 256 bytes
			return (sizeof(BufferStruct) + minbuffer_size) & ~minbuffer_size;
		}

		void Map()
		{
			CD3DX12_RANGE readRange(0, 0);    // We do not intend to read from this resource on the CPU. (End is less than or equal to begin)
			ThrowIfFailed(m_GpuBuffer->Map(0, &readRange, reinterpret_cast<void**>(&m_GpuAddress)));
		}

		bool Update()
		{
			if (m_GpuAddress)
			{
				memcpy(m_GpuAddress, m_CpuBuffer, sizeof(BufferStruct));
				return true;
			}
			return false;
		}

	};
}

#endif