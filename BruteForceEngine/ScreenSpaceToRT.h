#ifndef SCRREN_SPACE_TO_RT_H
#define SCRREN_SPACE_TO_RT_H
#include "RenderSubsystem.h"
namespace BruteForce
{
    namespace Render
    {
        class ScreenSpaceToRt : public RenderSubsystem
        {
        private:
            //DescriptorHeap m_SVRHeap;

            DescriptorHeap m_SrvHeapPtr;
            DescriptorHeap m_SamplerHeap;
            pResource m_IndexBuffer;
            IndexBufferView m_IndexBufferView;
        public:
            ScreenSpaceToRt();
            virtual ~ScreenSpaceToRt();
            virtual void Update(float delta_time, uint8_t frame_index);
            virtual void LoadContent(Device& device, uint8_t frames_count, const RenderSubsystemInitDesc&, SmartCommandQueue& copy_queue);
            virtual SmartCommandList& PrepareRenderCommandList(SmartCommandList&, const RenderDestination&);
            void SetRenderParameter(DescriptorHeap SrvPtr);
        };
    }
}
#endif
