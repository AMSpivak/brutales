#ifndef ENGINE_GPU_SWAP_CHAIN_H
#define ENGINE_GPU_SWAP_CHAIN_H
#include "PlatformDefine.h"
#include "EngineGpuCommands.h"

namespace BruteForce
{
    class SmartCommandQueue;
    SwapChain CreateSwapChain(SmartCommandQueue& commandQueue, uint32_t bufferCount, int Width, int Height, bool tearing, TargetFormat format, WindowHandle mhWnd);
    SwapChain CreateSwapChain(CommandQueue& commandQueue, uint32_t bufferCount, int Width, int Height, bool tearing, TargetFormat format, WindowHandle mhWnd);
}

#endif
