#include "EngineGpuCommands.h"
#include "Helpers.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include "d3dx12.h"
#include "VideoDriverInterface.h"

#ifdef _DEBUG
#define ENABLE_MARKERS
#endif

namespace BruteForce
{

    CommandQueue CreateCommandQueue(Device& device, CommandListType type)
    {
        CommandQueue d3d12CommandQueue;

        D3D12_COMMAND_QUEUE_DESC desc = {};
        desc.Type = type;
        desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
        desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        desc.NodeMask = 0;

        ThrowIfFailed(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&d3d12CommandQueue)));

        return d3d12CommandQueue;
    }

    CommandAllocator CreateCommandAllocator(Device& device,
        CommandListType type)
    {
        CommandAllocator commandAllocator;
        ThrowIfFailed(device->CreateCommandAllocator(type, IID_PPV_ARGS(&commandAllocator)));

        return commandAllocator;
    }

    GraphicsCommandList CreateCommandList(Device& device,
        CommandAllocator commandAllocator, D3D12_COMMAND_LIST_TYPE type)
    {
        GraphicsCommandList commandList;
        ThrowIfFailed(device->CreateCommandList(0, type, commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList)));
        return commandList;
    }


    void Flush(CommandQueue commandQueue, Fence fence,
        uint64_t& fenceValue, EventHandle fenceEvent)
    {
        uint64_t fenceValueForSignal = BruteForce::Sync::Signal(commandQueue, fence, fenceValue);
        BruteForce::Sync::WaitForFenceValue(fence, fenceValueForSignal, fenceEvent);
    }


    void Flush(CommandQueue commandQueue, BruteForce::Sync::SmartFence& fence)
    {
        uint64_t fenceValueForSignal = fence.Signal(commandQueue);
        fence.WaitForFenceValue(fenceValueForSignal);
    }

    SmartCommandQueue::SmartCommandQueue(Device& device, CommandListType type) 
        :m_command_queue(CreateCommandQueue(device, type)),
        m_fence(device),
        m_device(device),
        m_list_type(type)
    {
        //m_command_queue->SetName(LPCWSTR)
        //BruteForce::ReportLiveObjects();
    }

    SmartCommandQueue::~SmartCommandQueue()
    {
        //m_device->Release();
    }

	void SmartCommandQueue::SetName(LPCWSTR name)
	{
        ThrowIfFailed(m_command_queue->SetName(name));
	}

    SmartCommandList SmartCommandQueue::GetCommandList()
    {
        CommandAllocator command_allocator;
        if (!m_command_allocator_queue.empty() && m_fence.IsCompleted(m_command_allocator_queue.front().fence_value))
        {
            command_allocator = m_command_allocator_queue.front().command_allocator;
            m_command_allocator_queue.pop();

            ThrowIfFailed(command_allocator->Reset());
        }
        else
        {
            command_allocator = CreateCommandAllocator(m_device, m_list_type);
        }

        GraphicsCommandList command_list;

        if (!m_command_list_queue.empty())
        {
            command_list = m_command_list_queue.front();
            m_command_list_queue.pop();

            ThrowIfFailed(command_list->Reset(command_allocator.Get(), nullptr));
        }
        else
        {
            command_list = CreateCommandList(m_device, command_allocator, m_list_type);
        }

        return SmartCommandList(command_list, command_allocator);
    }




    uint64_t SmartCommandQueue::ExecuteCommandList(SmartCommandList& list)
    {
        list.command_list->Close();

        ID3D12CommandList* const ppCommandLists[] = {
            list.command_list.Get()
        };

        m_command_queue->ExecuteCommandLists(1, ppCommandLists);
        uint64_t fence_value = m_fence.Signal(m_command_queue);

        m_command_allocator_queue.emplace(CommandAllocatorEntry{ fence_value, list.command_allocator});
        m_command_list_queue.push(list.command_list);
        return fence_value;
    }

    void SmartCommandList::ClearRTV(BruteForce::DescriptorHandle& rtv, const FLOAT* clearColor)
    {
        command_list->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
    }

    void SmartCommandList::ClearDSV(BruteForce::DescriptorHandle& dsv, bool depth, bool stencil, FLOAT depth_val, UINT8 stencil_val)
    {
        D3D12_CLEAR_FLAGS flags = static_cast<D3D12_CLEAR_FLAGS>(0);

        if (depth)
        {
            flags |= D3D12_CLEAR_FLAG_DEPTH;
        }

        if (stencil)
        {
            flags |= D3D12_CLEAR_FLAG_STENCIL;
        }

        command_list->ClearDepthStencilView(dsv, flags, depth_val, stencil_val, 0, nullptr);
    }

    void SmartCommandList::SetPipelineState(const PipelineState& state)
    {
        command_list->SetPipelineState(state.Get());
    }

    void SmartCommandList::SetGraphicsRootSignature(const RootSignature& signature)
    {
        command_list->SetGraphicsRootSignature(signature.Get());
    }

    void SmartCommandList::SetComputeRootSignature(const RootSignature& signature)
    {
        command_list->SetComputeRootSignature(signature.Get());
    }

    void SmartCommandList::BeginEvent(UINT64 color, char const* formatString)
    {
#ifdef ENABLE_MARKERS
        PIXBeginEvent(command_list.Get(), color, formatString);
#endif
    }

    void SmartCommandList::EndEvent()
    {
#ifdef ENABLE_MARKERS
        PIXEndEvent(command_list.Get());
#endif
    }


    uint64_t SmartCommandQueue::Signal()
    {
        return m_fence.Signal(m_command_queue);
    }

    uint64_t SmartCommandQueue::Signal(Sync::SmartFence& fence)
    {
        return fence.Signal(m_command_queue);
    }

    void SmartCommandQueue::GpuWait(Sync::SmartFence& fence)
    {
        fence.GpuWait(m_command_queue);
    }

    void SmartCommandQueue::GpuWaitNext(Sync::SmartFence& fence)
    {
        fence.GpuWaitNext(m_command_queue);
    }

    bool SmartCommandQueue::IsFenceCompleted()
    {
        return m_fence.IsCompleted();
    }

    bool SmartCommandQueue::IsFenceCompleted(uint64_t fenceValue)
    {
        return m_fence.IsCompleted(fenceValue);
    }

    void SmartCommandQueue::WaitForFenceValue(uint64_t fenceValue, std::chrono::milliseconds duration)
    {
        m_fence.WaitForFenceValue(fenceValue, duration);
    }

    void SmartCommandQueue::CopyTextureSubresource(Resource& desttexture, uint32_t firstSubresource, uint32_t numSubresources, D3D12_SUBRESOURCE_DATA* subresourceData, ResourceStates finalResourceState)
    {
        if (desttexture)
        {
            auto copy_list = GetCommandList();
            //flush
            ResourceBarrier barrier = BruteForce::ResourceBarrier::Transition(
                desttexture.Get(),
                ResourceStateCommon,
                ResourceStateCopyDest);

            copy_list.command_list->ResourceBarrier(1, &barrier);

            UINT64 requiredSize = GetRequiredIntermediateSize(desttexture.Get(), firstSubresource, numSubresources);

            // Create a temporary (intermediate) resource for uploading the subresources
            Resource intermediateResource;
            auto hprop = HeapProperties(D3D12_HEAP_TYPE_UPLOAD);
            auto buffer = CD3DX12_RESOURCE_DESC::Buffer(requiredSize);
            ThrowIfFailed(m_device->CreateCommittedResource(
                &hprop,
                D3D12_HEAP_FLAG_NONE,
                &buffer,
                D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&intermediateResource)
            ));
            UpdateSubresources(copy_list.command_list.Get(), desttexture.Get(), intermediateResource.Get(), 0, firstSubresource, numSubresources, subresourceData);
            
            ResourceBarrier barrier_dest = BruteForce::ResourceBarrier::Transition(
                desttexture.Get(),
                ResourceStateCopyDest,
                finalResourceState);
            copy_list.command_list->ResourceBarrier(1, &barrier_dest);

            auto fence = ExecuteCommandList(copy_list);
            WaitForFenceValue(fence);
        }
    }

    void SmartCommandQueue::Flush()
    {
        BruteForce::Flush(m_command_queue, m_fence);
    }

}