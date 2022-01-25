#ifndef RENDER_TERRAIN_H
#define RENDER_TERRAIN_H
#include "RenderSubsystem.h"
#include "Texture.h"
#include "IndexedGeometry.h"
#include <vector>
#include <memory>
namespace BruteForce
{
    namespace Render
    {
        class RenderTerrain : public RenderSubsystem
        {
        private:
            std::vector<std::shared_ptr<Textures::Texture>> m_textures;
            DescriptorHeap m_SVRHeap;
            DescriptorHeap m_SamplerHeap;
            IndexedGeometry m_plane;
        public:
            RenderTerrain();
            virtual ~RenderTerrain();
            virtual void Update(float delta_time);
            virtual void LoadContent(Device& device);
            virtual SmartCommandList& PrepareRenderCommandList(SmartCommandList&, const RenderDestination&);
        };
    }
}


#endif
