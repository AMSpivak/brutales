#ifndef CALC_TERRAIN_SHADOW_H
#define CALC_TERRAIN_SHADOW_H
#include "ComputeSubsystem.h"
//#include "AAVasiliev/PlatformPoint.h"
#include "TerrainShadowCB.h"
#include "ConstantBuffer.h"
#include "Texture.h"
#include <memory>


namespace BruteForce
{
    namespace Compute
    {
        

        class ComputeTerrainShadow: public ComputeSubsystem
        {
        private:
            //std::shared_ptr<DescriptorHeapRange> SunShadowSrvDescriptors;
            std::shared_ptr<DescriptorHeapRange> HeightmapTexturesRange;
            std::shared_ptr<DescriptorHeapRange> SunShadowUavDescriptors;
            std::shared_ptr <DescriptorHeapRange> CbvRange;

            ConstantBuffer<TerrainShadowCB>* m_TerrainShadowBuffers;
        public:
            static const int GetTerrainShadowSize() { return 2048; }
            ComputeTerrainShadow();
            virtual ~ComputeTerrainShadow();
            virtual void Update(float delta_time, uint8_t frame_index);
            virtual void LoadContent(Device& device, uint8_t frames_count, DescriptorHeapManager& descriptor_heap_manager);
            virtual SmartCommandList& PrepareRenderCommandList(SmartCommandList&, const PrepareComputeHelper&);
        };
    }

}
#endif