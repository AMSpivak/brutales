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