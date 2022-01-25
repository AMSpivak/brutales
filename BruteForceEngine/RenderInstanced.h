#ifndef RENDER_INSTANCED_H
#define RENDER_INSTANCED_H
#include "RenderSubsystem.h"
#include "Texture.h"
#include "IndexedGeometry.h"
#include <array>
namespace BruteForce
{
    namespace Render
    {
        class RenderInstanced: public RenderSubsystem
        {
        private:
            std::array<Textures::Texture,2> m_textures;
            DescriptorHeap m_SVRHeap;
            DescriptorHeap m_SamplerHeap;
            IndexedGeometry m_cube;
        public:
            RenderInstanced();
            virtual ~RenderInstanced();
            virtual void Update(float delta_time);
            virtual void LoadContent(Device& device);
            virtual SmartCommandList& PrepareRenderCommandList(SmartCommandList&, const RenderDestination&);
        };
    }
}


#endif
