#ifndef RENDER_SKINNED_H
#define RENDER_SKINNED_H
#include "RenderSubsystem.h"
#include "Texture.h"
#include "RenderObject.h"
#include <memory>
#include <vector>
namespace BruteForce
{
    namespace Render
    {
        class RenderSkinned : public RenderSubsystem
        {
        private:
            DescriptorHeap m_SamplerHeap;
            std::vector <RenderObject> m_objects;
            std::shared_ptr<DescriptorHeapRange> TexturesRange;
        public:
            RenderSkinned();
            virtual ~RenderSkinned();
            virtual void Update(float delta_time, uint8_t frame_index);
            virtual void LoadContent(Device& device, uint8_t frames_count, const RenderSubsystemInitDesc&, SmartCommandQueue& copy_queue, DescriptorHeapManager& descriptor_heap_manager);
            virtual SmartCommandList& PrepareRenderCommandList(SmartCommandList&, const PrepareRenderHelper&);
        };
    }
}


#endif

