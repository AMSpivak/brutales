#ifndef RENDER_INSTANCED_H
#define RENDER_INSTANCED_H
#include "RenderSubsystem.h"
#include "Texture.h"
#include "RenderObject.h"
#include <memory>
#include <vector>
namespace BruteForce
{
    namespace Render
    {
        class RenderInstanced: public RenderSubsystem
        {
        private:
            std::vector<std::shared_ptr<Textures::Texture>> m_textures;
            DescriptorHeap m_SVRHeap;
            DescriptorHeap m_SamplerHeap;
            //IndexedGeometry m_cube;
            std::vector <RenderObject> m_objects;
            std::shared_ptr<DescriptorHeapRange> TexturesRange;
        public:
            RenderInstanced();
            virtual ~RenderInstanced();
            virtual void Update(float delta_time, uint8_t frame_index);
            virtual void LoadContent(Device& device, uint8_t frames_count, const RenderSubsystemInitDesc&, SmartCommandQueue& copy_queue, DescriptorHeapManager& descriptor_heap_manager);
            virtual SmartCommandList& PrepareRenderCommandList(SmartCommandList&, const PrepareRenderHelper&);
        };
    }
}


#endif
