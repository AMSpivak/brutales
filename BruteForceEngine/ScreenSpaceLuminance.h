#ifndef SCRREN_SPACE_LUMINANCE_H
#define SCRREN_SPACE_LUMINANCE_H

#include "CommonRenderParams.h"
#include "RenderSubsystem.h"
#include "ConstantBuffer.h"
namespace BruteForce
{
    namespace Render
    {
        class ScreenSpaceLuminance : public RenderSubsystem
        {
        private:
            std::shared_ptr<DescriptorHeapRange> RTSrvDescriptors;
        public:
            ScreenSpaceLuminance();
            virtual ~ScreenSpaceLuminance();
            virtual void Update(float delta_time, uint8_t frame_index);
            virtual void LoadContent(Device& device, uint8_t frames_count, const RenderSubsystemInitDesc&, SmartCommandQueue& copy_queue, DescriptorHeapManager& descriptor_heap_manager);
            virtual SmartCommandList& PrepareRenderCommandList(SmartCommandList&, const PrepareRenderHelper&);
        };
    }
}
#endif
