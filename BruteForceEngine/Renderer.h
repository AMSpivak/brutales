#ifndef RENDERER_H
#define RENDERER_H

#include <chrono>


#include "PlatformDefine.h"
#include "VideoDriverDX12.h"
#include "EngineGpuFence.h"
#include "EngineGpuCommands.h"
#include "Camera.h"
#include "Helpers.h"

namespace BruteForce
{


    template <const uint8_t t_NumFrames>
    class Renderer
    {
    protected:
        BruteForce::Window* m_Window;
        const uint8_t m_NumFrames;
        const  BruteForce::TargetFormat m_TargetFormat;
        HDRMode::HDRMode m_HDRmode;
    public:
        // Use WARP adapter
        Device& m_Device;
        Adapter& m_Adapter;
        GpuAllocator m_GpuAllocator;
        SmartCommandQueue m_SmartCommandQueue;
        SmartCommandQueue m_ComputeSmartCommandQueue;
        Resource m_BackBuffers[t_NumFrames];
        DescriptorHeap m_BackBuffersDHeap;
        UINT m_RTVDescriptorSize;
        UINT m_CurrentBackBufferIndex;
        uint64_t m_FrameFenceValues[t_NumFrames] = {};
        Viewport m_Viewport;
        ScissorRect m_ScissorRect;
        Renderer(BruteForce::Device& device, BruteForce::Adapter& adapter, BruteForce::Window* pWindow, bool UseWarp, BruteForce::TargetFormat t_format) : m_Window(pWindow), m_NumFrames(t_NumFrames)
                , m_Device(device), m_Adapter(adapter),  m_TargetFormat(t_format), m_HDRmode(HDRMode::OFF)
                , m_SmartCommandQueue(m_Device, BruteForce::CommandListTypeDirect), m_ComputeSmartCommandQueue(m_Device, BruteForce::CommandListTypeCompute)
        {
            m_SmartCommandQueue.SetName(L"GeneralQueue");
			m_ComputeSmartCommandQueue.SetName(L"ComputeQueue");
            m_GpuAllocator = BruteForce::CreateGpuAllocator(m_Adapter, m_Device);

            m_Window->CreateSwapChain(m_SmartCommandQueue, m_NumFrames, m_TargetFormat);
            auto& refSwapChain = m_Window->GetSwapChainReference();

            m_CurrentBackBufferIndex = refSwapChain->GetCurrentBackBufferIndex();

            m_BackBuffersDHeap = BruteForce::CreateDescriptorHeap(m_Device, BruteForce::DescriptorHeapRTV, m_NumFrames);
            m_RTVDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(BruteForce::DescriptorHeapRTV);

            BruteForce::UpdateRenderTargetViews(m_Device, refSwapChain, m_BackBuffersDHeap, m_BackBuffers, m_NumFrames);
        }
        void Flush() { m_ComputeSmartCommandQueue.Flush();  m_SmartCommandQueue.Flush(); }
        ~Renderer() 
        {
            //ReportLiveObjects();
        }

        bool SetHDR(HDRMode::HDRMode mode)
        {
            uint32_t width = m_Window->GetWidth();
            uint32_t height = m_Window->GetHeight();

            auto& refSwapChain = m_Window->GetSwapChainReference();
            BruteForce::SwapChainDesc swapChainDesc = {};
            ThrowIfFailed(refSwapChain->GetDesc(&swapChainDesc));
            auto swch_format = swapChainDesc.BufferDesc.Format;
            decltype(swch_format) new_format = TargetFormat_R8G8B8A8_Unorm;
            switch(mode)
            {
            case HDRMode::OFF:
                new_format = TargetFormat_R8G8B8A8_Unorm;
                break;
            case HDRMode::RGB_FULL_G2084_NONE_P2020:
                new_format = TargetFormat_R10G10B10A2_Unorm;

                break;
            case HDRMode::RGB_FULL_G22_NONE_P709:
                new_format = TargetFormat_R16G16B16A16_Float;

                break;
            default:
                assert(false);
                break;
            }

            if (swch_format == new_format)
                return false;

            m_HDRmode = mode;

            //auto ret = m_Window->SetHDRMode(mode);
            ThrowIfFailed(refSwapChain->ResizeBuffers(GetBuffersCount(), width, height,
                new_format, swapChainDesc.Flags));
            return m_Window->SetHDRMode(mode);
        }

        void Resize()
        {
            uint32_t width = m_Window->GetWidth();
            uint32_t height = m_Window->GetHeight();
            // Flush the GPU queue to make sure the swap chain's back buffers
            // are not being referenced by an in-flight command list.
            Flush();
            //m_SmartCommandQueue.Flush();

            for (int i = 0; i < GetBuffersCount(); ++i)
            {
                // Any references to the back buffers must be released
                // before the swap chain can be resized.
                m_BackBuffers[i].Reset();
                m_FrameFenceValues[i] = m_FrameFenceValues[m_CurrentBackBufferIndex];
            }

            auto& refSwapChain = m_Window->GetSwapChainReference();

            BruteForce::SwapChainDesc swapChainDesc = {};
            ThrowIfFailed(refSwapChain->GetDesc(&swapChainDesc));

            

            ThrowIfFailed(refSwapChain->ResizeBuffers(GetBuffersCount(), width, height,
                swapChainDesc.BufferDesc.Format, swapChainDesc.Flags));

            bool can_hdr = m_Window->IsOnHDRDisplay(m_Adapter);

            if (can_hdr)
            {
                SetHDR(HDRMode::RGB_FULL_G22_NONE_P709);
            }

            m_CurrentBackBufferIndex = refSwapChain->GetCurrentBackBufferIndex();
            BruteForce::UpdateRenderTargetViews(m_Device, refSwapChain, m_BackBuffersDHeap, m_BackBuffers, GetBuffersCount());
            
        }

        const uint8_t GetBuffersCount() { return m_NumFrames; }
        void SetCurrentFenceValue(uint64_t value) { m_FrameFenceValues[m_CurrentBackBufferIndex] = value; };
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

		void WaitForSwapReadyFence(BruteForce::SmartCommandQueue& in_SmartCommandQueue)
		{
            auto i = m_CurrentBackBufferIndex;// (m_CurrentBackBufferIndex == 0 ? t_NumFrames : m_CurrentBackBufferIndex) - 1;

			in_SmartCommandQueue.WaitForFenceValue(m_FrameFenceValues[i]);
		}

        virtual Camera* GetCameraPtr() { return nullptr; }
    };
}

#endif
