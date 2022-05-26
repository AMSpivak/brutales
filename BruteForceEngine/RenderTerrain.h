#ifndef RENDER_TERRAIN_H
#define RENDER_TERRAIN_H
#include "RenderSubsystem.h"
#include "Texture.h"
#include "IndexedGeometry.h"
#include <vector>
#include <memory>
#include "ConstantBuffer.h"
namespace BruteForce
{
    namespace Render
    {
        class RenderTerrain : public RenderSubsystem
        {
        private:
            struct TerrainCB
            {               
                Math::Vec4Float m_PlanesPositions[1024];
                Math::Vec4Float m_TerrainScaler;
            };
            ConstantBuffer<TerrainCB>* m_TerrainBuffers;
            std::vector<std::shared_ptr<Textures::Texture>> m_textures;
            DescriptorHeap m_SVRHeap;
            DescriptorHeap m_SamplerHeap;
            IndexedGeometry m_plane;
            UINT PreparePlanesCB(const Math::Vec4Float &cam, uint32_t index);
        public:
            RenderTerrain();
            virtual ~RenderTerrain();
            virtual void Update(float delta_time, uint8_t frame_index);
            virtual void LoadContent(Device& device, uint8_t frames_count, const RenderSubsystemInitDesc&, SmartCommandQueue& copy_queue);
            virtual SmartCommandList& PrepareRenderCommandList(SmartCommandList&, const RenderDestination&);
        };
    }
}


#endif
