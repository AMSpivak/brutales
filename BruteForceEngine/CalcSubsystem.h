#ifndef CALC_SUBSYSTEM_H
#define CALC_SUBSYSTEM_H
#include "PlatformDefine.h"
#include "EngineGpuCommands.h"
#include "Camera.h"

namespace BruteForce
{
    namespace Render
    {
        class CalcSubsystem
        {
        protected:

            bool m_loaded;
            uint8_t m_FramesCount;
            RootSignature m_RootSignature;
            PipelineState m_PipelineState;
        public:
            CalcSubsystem() :m_loaded(false) {};
            virtual ~CalcSubsystem()
            {
                //if (m_RootSignature) m_RootSignature->Release();
                //if (m_PipelineState) m_PipelineState->Release();
            };
            virtual void Update(float delta_time, uint8_t frame_index) = 0;
            virtual void LoadContent(Device& device, uint8_t frames_count) = 0;
            virtual SmartCommandList& PrepareRenderCommandList(SmartCommandList&) = 0;
        };
    }

}
#endif