#ifndef RENDERER_H
#define RENDERER_H
#include "PlatformDefine.h"
#include "VideoDriverDX12.h"
#include "EngineGpuFence.h"
#include "EngineGpuCommands.h"

namespace BruteForce
{
    template <const uint8_t t_NumFrames>
    class Renderer
    {
    private:
        const uint8_t m_NumFrames;
    public:
        // Use WARP adapter
        BruteForce::Device& m_Device;
        BruteForce::SmartCommandQueue m_SmartCommandQueue;
        BruteForce::Resource m_BackBuffers[t_NumFrames];
        BruteForce::DescriptorHeap m_RTVDescriptorHeap;
        UINT m_RTVDescriptorSize;
        UINT m_CurrentBackBufferIndex;
        uint64_t m_FrameFenceValues[t_NumFrames] = {};
        Renderer(BruteForce::Device& device):m_SmartCommandQueue(m_Device, BruteForce::CommandListTypeDirect), m_NumFrames(t_NumFrames),m_Device(device) {}
        ~Renderer() { m_SmartCommandQueue.Flush(); }
        void Init(BruteForce::Window* pWindow, bool UseWarp)
        {
            pWindow->CreateSwapChain(m_SmartCommandQueue, m_NumFrames);

            auto refSwapChain = pWindow->GetSwapChainReference();

            m_CurrentBackBufferIndex = refSwapChain->GetCurrentBackBufferIndex();

            m_RTVDescriptorHeap = BruteForce::CreateDescriptorHeap(m_Device, BruteForce::DescriptorHeapRTV, m_NumFrames);
            m_RTVDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(BruteForce::DescriptorHeapRTV);

            BruteForce::UpdateRenderTargetViews(m_Device, refSwapChain, m_RTVDescriptorHeap, m_BackBuffers, m_NumFrames);
        }
    };
}


#endif
