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
        };

        class RenderSubsystem
        {
        protected:
            bool m_loaded;
            RootSignature m_RootSignature;
            PipelineState m_PipelineState;
        public:
            RenderSubsystem():m_loaded(false) {};
            virtual ~RenderSubsystem()
            {
                //if (m_RootSignature) m_RootSignature->Release();
                //if (m_PipelineState) m_PipelineState->Release();
            };
            virtual void Update(float delta_time) = 0;
            virtual void LoadContent(Device& device) = 0;
            virtual SmartCommandList& PrepareRenderCommandList(SmartCommandList&, const RenderDestination&) = 0;
        };
    }

}
#endif
