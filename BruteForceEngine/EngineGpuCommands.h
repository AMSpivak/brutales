#ifndef ENGINE_GPU_COMMANDS_H
#define ENGINE_GPU_COMMANDS_H
#include "PlatformDefine.h"
#include "EngineGpuFence.h"
#include "EngineGpuSwapChain.h"
#include <queue> 
namespace BruteForce
{
    CommandQueue CreateCommandQueue(Device device, CommandListType type);
    CommandAllocator CreateCommandAllocator(Device device, CommandListType type);
    GraphicsCommandList CreateCommandList(Device device, CommandAllocator commandAllocator, CommandListType type);
    void Flush(CommandQueue commandQueue, Fence fence,
        uint64_t& fenceValue, EventHandle fenceEvent);
    void Flush(CommandQueue commandQueue, SmartFence& fence);

    class SmartCommandQueue;

    class SmartCommandList
    {
    public:
        GraphicsCommandList command_list;
    private:
        SmartCommandList(GraphicsCommandList list, CommandAllocator allocator) :command_list(list), command_allocator(allocator) {}
        CommandAllocator command_allocator;
        friend class SmartCommandQueue;
    public: 
        void ClearRTV(DescriptorHandle& rtv, const FLOAT* clearColor);
        void ClearDSV(DescriptorHandle& dsv, bool depth, bool stencil, FLOAT depth_val, UINT8 stencil_val);
        void SetPipelineState(const PipelineState& state);
        void SetRootSignature(const RootSignature& signature);
    };



    class SmartCommandQueue
    {
    public:
        SmartCommandQueue(Device device, CommandListType type);
        ~SmartCommandQueue();
        void Flush();
        SmartCommandList GetCommandList();
        uint64_t ExecuteCommandList(SmartCommandList& list);
        uint64_t Signal();
        void WaitForFenceValue(uint64_t fenceValue, std::chrono::milliseconds duration = std::chrono::milliseconds::max());
        void CopyTextureSubresource(Resource& desttexture, uint32_t firstSubresource, uint32_t numSubresources, D3D12_SUBRESOURCE_DATA* subresourceData, ResourceStates finalResourceState);


        //CommandQueue m_command_queue;
    private:
        struct CommandAllocatorEntry
        {
            uint64_t fence_value;
            CommandAllocator command_allocator;
        };


        std::queue<CommandAllocatorEntry> m_command_allocator_queue;
        std::queue<GraphicsCommandList> m_command_list_queue;

        CommandQueue m_command_queue;
        SmartFence m_fence;
        Device m_device;
        CommandListType m_list_type;

        friend SwapChain CreateSwapChain(SmartCommandQueue& commandQueue, uint32_t bufferCount, int Width, int Height, bool tearing, WindowHandle mhWnd);
    };
}
#endif