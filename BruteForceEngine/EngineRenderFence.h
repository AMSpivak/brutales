#ifndef ENGINE_RENDER_FENCE_H
#define ENGINE_RENDER_FENCE_H
#include "VideoDriverInterface.h"
namespace BruteForce
{
	Fence CreateFence(Device device);
	uint64_t Signal(CommandQueue commandQueue, Fence fence, uint64_t& fenceValue);
	EventHandle CreateEventHandle();
}
#endif
