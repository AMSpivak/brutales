#ifndef SCREEN_SPACE_SKY_H
#define SCREEN_SPACE_SKY_H
#include "RenderSubsystem.h"
#include "ConstantBuffer.h"

namespace BruteForce
{
    namespace Render
    {
        class ScreenSpaceSky : public RenderSubsystem
        {
        private:
//#ifdef PLATFORM_DX12_H
//            using float4 = BruteForce::Math::Vec4Float;
//#endif
            //using float4 = BruteForce::Math::Vec4Float;
#include "SkyPixelCB.h"

            ConstantBuffer<SkyPixelCB>* m_SkyBuffers;
            std::shared_ptr <DescriptorHeapRange> CbvRange;
        public:
            ScreenSpaceSky() :m_SkyBuffers{ nullptr } {};
            virtual ~ScreenSpaceSky();
            virtual void Update(float delta_time, uint8_t frame_index);
            virtual void LoadContent(Device& device, uint8_t frames_count, const RenderSubsystemInitDesc&, SmartCommandQueue& copy_queue, DescriptorHeapManager& descriptor_heap_manager);
            virtual SmartCommandList& PrepareRenderCommandList(SmartCommandList&, const PrepareRenderHelper&);
        };
    }
}
#endif
