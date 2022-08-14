#ifndef CALC_SUBSYSTEM_H
#define CALC_SUBSYSTEM_H
#include "PlatformDefine.h"
#include "EngineGpuCommands.h"
#include "Camera.h"
#include "DescriptorHeapManager.h"


namespace BruteForce
{
    namespace Compute
    {
        struct PrepareComputeHelper
        {
            const Viewport* m_Viewport;
            const ScissorRect* m_ScissorRect;
            //const BruteForce::DescriptorHandle* rtv;
            //const BruteForce::DescriptorHandle* dsv;
            const Camera& camera;
            const uint8_t frame_index;
            DescriptorHeapManager& HeapManager;
        };

        class ComputeSubsystem
        {
        protected:

            bool m_loaded;
            uint8_t m_FramesCount;
            RootSignature m_RootSignature;
            PipelineState m_PipelineState;
        public:
            ComputeSubsystem() :m_loaded(false), m_FramesCount(0) {};
            virtual ~ComputeSubsystem()
            {
                //if (m_RootSignature) m_RootSignature->Release();
                //if (m_PipelineState) m_PipelineState->Release();
            };
            virtual void Update(float delta_time, uint8_t frame_index) = 0;
            virtual void LoadContent(Device& device, uint8_t frames_count, DescriptorHeapManager& descriptor_heap_manager) = 0;
            virtual SmartCommandList& PrepareRenderCommandList(SmartCommandList&, const PrepareComputeHelper&) = 0;
        };
    }

}
#endif