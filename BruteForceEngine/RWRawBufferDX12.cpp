#include "RWRawBuffer.h"

namespace BruteForce
{
	namespace Buffers
	{
		void RWRawBuffer::Create(Device& device, size_t size, GpuAllocator gpu_allocator)
		{
			ResourceDesc res_desc = {};
			CreateBufferResource(device, *this, &res_desc, nullptr, gpu_allocator);
			m_size = size;
		}
		void RWRawBuffer::CreateSrv(Device& device, DescriptorHeapRange& descriptor_range, size_t index)
		{
		}
		void RWRawBuffer::CreateUav(Device& device, DescriptorHeapRange& descriptor_range, size_t index)
		{
		}
	}
}