#ifndef RENDER_TERRAIN_H
#define RENDER_TERRAIN_H
#include "RenderSubsystem.h"
#include "Texture.h"
#include "IndexedGeometry.h"
#include <vector>
#include <memory>
#include "ConstantBuffer.h"
//#include "ShaderCppTypesRedefine.h"


namespace BruteForce
{
    namespace Render
    {
        class RenderTerrain : public RenderSubsystem
        {
        private:
            #include "ShaderCppTypesRedefine.h"
            #include "TerrainVertexCB.h"
            ConstantBuffer<TerrainVertexCB>* m_TerrainBuffers;
            std::vector<std::shared_ptr<Textures::Texture>> m_textures;
            DescriptorHeap m_SVRHeap;
            DescriptorHeap m_SamplerHeap;
            IndexedGeometry m_plane;
            UINT PreparePlanesCB(const Math::Vec4Float &cam, uint32_t index);
            std::shared_ptr<DescriptorHeapRange> TexturesRange;
            std::shared_ptr<DescriptorHeapRange> HeightmapTexturesRange;
            std::shared_ptr <DescriptorHeapRange> CbvRange;
            std::shared_ptr <DescriptorHeapRange> SunShadowSrvDescriptors;
        public:
            RenderTerrain();
            virtual ~RenderTerrain();
            virtual void Update(float delta_time, uint8_t frame_index);
            virtual void LoadContent(Device& device, uint8_t frames_count, const RenderSubsystemInitDesc&, SmartCommandQueue& copy_queue, DescriptorHeapManager& descriptor_heap_manager);
            virtual SmartCommandList& PrepareRenderCommandList(SmartCommandList&, const PrepareRenderHelper&);
        };
    }
}


#endif
