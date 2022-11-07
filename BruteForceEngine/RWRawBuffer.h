#ifndef RWRAW_BUFFER
#define RWRAW_BUFFER

#include "Resources.h"

namespace BruteForce
{
	namespace Buffers
	{
		struct BufferLoadHlpr
		{
			Device& m_device;
			SmartCommandQueue& m_copy_queue;
			GpuAllocator m_gpu_allocator;
			BufferLoadHlpr(Device& device, SmartCommandQueue& copy_queue, GpuAllocator gpu_allocator) :m_device(device), m_copy_queue(copy_queue), m_gpu_allocator(gpu_allocator) {}
		};

		class RWRawBuffer : public GpuResource
		{
		private:
			size_t m_size;
			//std::mutex m_mutex;
		public:
			RWRawBuffer()  {};
			RWRawBuffer(const RWRawBuffer&) = default;
			~RWRawBuffer()
			{};
			void Create(Device& device, size_t size, GpuAllocator gpu_allocator = nullptr);
			//void CreateSrv(Device& device, DescriptorHandle& descriptor_handle);
			void CreateSrv(Device& device, DescriptorHeapRange& descriptor_range, size_t index);
			//void CreateUav(Device& device, DescriptorHandle& descriptor_handle);
			void CreateUav(Device& device, DescriptorHeapRange& descriptor_range, size_t index);
		};

	}
}


#endif
