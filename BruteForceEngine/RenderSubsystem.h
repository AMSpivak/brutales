#ifndef RENDER_SUBSYSTEM_H
#define RENDER_SUBSYSTEM_H
#include "PlatformDefine.h"
#include "EngineGpuCommands.h"
#include "Camera.h"

namespace BruteForce
{
    namespace Render
    {
        struct RenderDestination
        {
            const Viewport* m_Viewport;
            const ScissorRect* m_ScissorRect;
            const BruteForce::DescriptorHandle* rtv;
            const BruteForce::DescriptorHandle* dsv;
            const Camera& camera;
            const uint8_t frame_index;
        };

        class RenderSubsystem
        {
        protected:
            
            bool m_loaded;
            uint8_t m_FramesCount;
            RootSignature m_RootSignature;
            PipelineState m_PipelineState;
        public:
            RenderSubsystem():m_loaded(false) {};
            virtual ~RenderSubsystem()
            {
                //if (m_RootSignature) m_RootSignature->Release();
                //if (m_PipelineState) m_PipelineState->Release();
            };
            virtual void Update(float delta_time, uint8_t frame_index) = 0;
            virtual void LoadContent(Device& device, uint8_t frames_count) = 0;
            virtual SmartCommandList& PrepareRenderCommandList(SmartCommandList&, const RenderDestination&) = 0;
        };
    }

}
#endif
