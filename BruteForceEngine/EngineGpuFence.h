#ifndef ENGINE_GPU_FENCE_H
#define ENGINE_GPU_FENCE_H
#include "PlatformDefine.h"

#include <chrono>

#if defined(min)
#undef min
#endif

#if defined(max)
#undef max
#endif

namespace BruteForce
{
	namespace Sync
	{
		Fence CreateFence(Device & device);
		uint64_t Signal(CommandQueue commandQueue, Fence fence, uint64_t & fenceValue);
		EventHandle CreateEventHandle();
		void CloseEventHandle(EventHandle & handle);
		void WaitForFenceValue(Fence fence, uint64_t fenceValue, HANDLE fenceEvent, std::chrono::milliseconds duration = std::chrono::milliseconds::max());
		void GpuWait(CommandQueue commandQueue, Fence fence, uint64_t fenceValue);

		
	

		class SmartFence
		{
		public:
			SmartFence(Device& device);
			~SmartFence();
			uint64_t Signal(CommandQueue commandQueue);
			void GpuWait(CommandQueue commandQueue);
			bool IsCompleted();
			bool IsCompleted(uint64_t fenceValue);
			void WaitForFenceValue(uint64_t fenceValue, std::chrono::milliseconds duration = std::chrono::milliseconds::max());
		private:
			Fence m_fence;
			EventHandle m_handle;
			uint64_t m_fence_value;
		};
	}
}
#endif
