#ifndef CONSTANT_BUFFER_H
#define CONSTANT_BUFFER_H

#include "PlatformDefine.h"

namespace BruteForce
{
	template <class BufferStruct>
	class ConstantBuffer
	{
	private:
		Resource m_GpuBuffer;
		BufferStruct* m_CpuBuffer;
	public:

		ConstantBuffer()
		{
			m_CpuBuffer = new BufferStruct();
			ZeroMemory(m_CpuBuffer, sizeof(BufferStruct));
		}

		~ConstantBuffer()
		{
			delete m_CpuBuffer;
		}
	};
}

#endif