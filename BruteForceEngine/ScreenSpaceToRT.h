#ifndef SCRREN_SPACE_TO_RT_H
#define SCRREN_SPACE_TO_RT_H

#include "CommonRenderParams.h"
#include "RenderSubsystem.h"
#include "ConstantBuffer.h"
namespace BruteForce
{
    namespace Render
    {
        class ScreenSpaceToRt : public RenderSubsystem
        {
        private:
            std::shared_ptr<DescriptorHeapRange> RTSrvDescriptors;
            std::shared_ptr<DescriptorHeapRange> CbvRange;
#include "TonemapCB.h"
            ConstantBuffer<TonemapCB>* m_TonemapBuffers;

            void PrepareCB(uint32_t index);
        public:
            ScreenSpaceToRt();
            virtual ~ScreenSpaceToRt();
            virtual void Update(float delta_time, uint8_t frame_index);
            virtual void LoadContent(Device& device, uint8_t frames_count, const RenderSubsystemInitDesc&, SmartCommandQueue& copy_queue, DescriptorHeapManager& descriptor_heap_manager);
            virtual SmartCommandList& PrepareRenderCommandList(SmartCommandList&, const PrepareRenderHelper&);
        };
    }
}
#endif
