#ifndef RENDER_INSTANCED_H
#define RENDER_INSTANCED_H
#include "RenderSubsystem.h"
#include "Texture.h"
#include "IndexedGeometry.h"
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
            IndexedGeometry m_cube;
        public:
            RenderInstanced();
            virtual ~RenderInstanced();
            virtual void Update(float delta_time, uint8_t frame_index);
            virtual void LoadContent(Device& device, uint8_t frames_count, const RenderSubsystemInitDesc&);
            virtual SmartCommandList& PrepareRenderCommandList(SmartCommandList&, const RenderDestination&);
        };
    }
}


#endif
