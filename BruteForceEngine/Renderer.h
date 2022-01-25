#ifndef RENDERER_H
#define RENDERER_H

#include <chrono>


#include "PlatformDefine.h"
#include "VideoDriverDX12.h"
#include "EngineGpuFence.h"
#include "EngineGpuCommands.h"
#include "Camera.h"

namespace BruteForce
{
    template <const uint8_t t_NumFrames>
    class Renderer
    {
    protected:
        BruteForce::Window* m_Window;
        const uint8_t m_NumFrames;
    public:
        // Use WARP adapter
        Device& m_Device;
        SmartCommandQueue m_SmartCommandQueue;
        Resource m_BackBuffers[t_NumFrames];
        DescriptorHeap m_BackBuffersDHeap;
        UINT m_RTVDescriptorSize;
        UINT m_CurrentBackBufferIndex;
        uint64_t m_FrameFenceValues[t_NumFrames] = {};
        RootSignature m_RootSignature;

        // Pipeline state object.
        PipelineState m_PipelineState;

        Viewport m_Viewport;
        ScissorRect m_ScissorRect;
        Renderer(BruteForce::Device& device, BruteForce::Window* pWindow, bool UseWarp) : m_Window(pWindow), m_NumFrames(t_NumFrames), m_Device(device)
                , m_SmartCommandQueue(m_Device, BruteForce::CommandListTypeDirect)
        {
            m_Window->CreateSwapChain(m_SmartCommandQueue, m_NumFrames);
            auto& refSwapChain = m_Window->GetSwapChainReference();

            m_CurrentBackBufferIndex = refSwapChain->GetCurrentBackBufferIndex();

            m_BackBuffersDHeap = BruteForce::CreateDescriptorHeap(m_Device, BruteForce::DescriptorHeapRTV, m_NumFrames);
            m_RTVDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(BruteForce::DescriptorHeapRTV);

            BruteForce::UpdateRenderTargetViews(m_Device, refSwapChain, m_BackBuffersDHeap, m_BackBuffers, m_NumFrames);
        }
        void Flush() { m_SmartCommandQueue.Flush(); }
        ~Renderer() 
        {
            //ReportLiveObjects();
        }

        const uint8_t GetBuffersCount() { return m_NumFrames; }
        void SetCurrentFence(uint64_t value) { m_FrameFenceValues[m_CurrentBackBufferIndex] = value; };
        uint64_t GetCurrentFence() { return m_FrameFenceValues[m_CurrentBackBufferIndex]; };
        BruteForce::Resource& GetCurrentBackBufferRef() { return m_BackBuffers[m_CurrentBackBufferIndex]; };

        void StartFrame(SmartCommandList& smart_command_list)
        {
            auto& backBuffer = GetCurrentBackBufferRef();
            BruteForce::ResourceBarrier barrier = BruteForce::ResourceBarrier::Transition(
                backBuffer.Get(),
                BruteForce::ResourceStatesPresent,
                BruteForce::ResourceStatesRenderTarget);

            smart_command_list.command_list->ResourceBarrier(1, &barrier);
        }

        void PrepareSwapFrame(SmartCommandList& smart_command_list)
        {
            auto& backBuffer = GetCurrentBackBufferRef();
            BruteForce::ResourceBarrier barrier = BruteForce::ResourceBarrier::Transition(
                backBuffer.Get(),
                BruteForce::ResourceStatesRenderTarget,
                BruteForce::ResourceStatesPresent);
            smart_command_list.command_list->ResourceBarrier(1, &barrier);

            /*UINT syncInterval = m_Window->GetVSync() ? 1 : 0;
            UINT presentFlags = m_Window->GetTearing() && !m_Window->GetVSync() ? AllowTearing : 0;
            auto& refSwapChain = m_Window->GetSwapChainReference();
            ThrowIfFailed(refSwapChain->Present(syncInterval, presentFlags));
            m_CurrentBackBufferIndex = refSwapChain->GetCurrentBackBufferIndex();*/
        }

        void SwapFrame()
        {
            UINT syncInterval = m_Window->GetVSync() ? 1 : 0;
            UINT presentFlags = m_Window->GetTearing() && !m_Window->GetVSync() ? AllowTearing : 0;
            auto& refSwapChain = m_Window->GetSwapChainReference();
            ThrowIfFailed(refSwapChain->Present(syncInterval, presentFlags));
            m_CurrentBackBufferIndex = refSwapChain->GetCurrentBackBufferIndex();
        }
        void WaitForCurrentFence(BruteForce::SmartCommandQueue& in_SmartCommandQueue)
        {
            in_SmartCommandQueue.WaitForFenceValue(GetCurrentFence());
        }

        virtual Camera* GetCameraPtr() { return nullptr; }
    };
}

#endif
