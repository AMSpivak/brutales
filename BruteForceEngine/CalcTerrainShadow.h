#ifndef CALC_TERRAIN_SHADOW_H
#define CALC_TERRAIN_SHADOW_H
#include "CalcSubsystem.h"
#include "AAVasiliev/PlatformPoint.h"
#include "TerrainShadowCB.h"
#include "ConstantBuffer.h"


namespace BruteForce
{
    namespace Render
    {
        class CalcTerrainShadow: public CalcSubsystem
        {
        protected:

            bool m_loaded;
            uint8_t m_FramesCount;
            RootSignature m_RootSignature;
            PipelineState m_PipelineState;
            ConstantBuffer<TerrainShadowCB>* m_TerrainShadowBuffers;
        public:
            CalcTerrainShadow();
            virtual ~CalcTerrainShadow();
            virtual void Update(float delta_time, uint8_t frame_index);
            virtual void LoadContent(Device& device, uint8_t frames_count);
            virtual SmartCommandList& PrepareRenderCommandList(SmartCommandList&);
        };
    }

}
#endif