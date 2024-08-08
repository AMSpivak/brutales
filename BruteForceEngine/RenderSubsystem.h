#ifndef RENDER_SUBSYSTEM_H
#define RENDER_SUBSYSTEM_H
#include "VideoDriverInterface.h"
#include "PlatformDefine.h"
#include "EngineGpuCommands.h"
#include "Camera.h"
#include "DescriptorHeapManager.h"
#include "MaterialManager.h"
#include <memory>

namespace BruteForce
{
    namespace Render
    {
        struct PrepareRenderHelper
        {
            const Viewport* m_Viewport;
            const ScissorRect* m_ScissorRect;
            const BruteForce::DescriptorHandle* rtv;
            const  uint8_t m_rt_count;
            const BruteForce::DescriptorHandle* dsv;
            const Camera& camera;
            const uint8_t frame_index;
            const  uint8_t rt_index;
            const float m_Nits;
            DescriptorHeapManager& HeapManager;
            std::shared_ptr<MaterialManager> mMaterialManager;
       };

        struct RenderSubsystemInitDesc
        {
            const TargetFormat RTFormat;
            const TargetFormat DepthFormat;
            GpuAllocator gpu_allocator_ptr;
            std::shared_ptr <BruteForce::MaterialManager> m_MaterialManager;
        };

        class RenderSubsystem
        {
        protected:

            bool m_loaded;
            uint8_t m_FramesCount;
            std::shared_ptr <BruteForce::PSO_RS> m_PsoRs;
            //RootSignature m_RootSignature;
            PipelineState m_PipelineState;
        public:
            RenderSubsystem() :m_loaded{ false }, m_FramesCount{0} { m_PsoRs = std::make_shared<BruteForce::PSO_RS>(); };
            virtual ~RenderSubsystem()
            {
            };
            virtual void Update(float delta_time, uint8_t frame_index) = 0;
            virtual void LoadContent(Device& device, uint8_t frames_count, const RenderSubsystemInitDesc&, SmartCommandQueue& copy_queue, DescriptorHeapManager &descriptor_heap_manager) = 0;
            virtual SmartCommandList& PrepareRenderCommandList(SmartCommandList&, const PrepareRenderHelper&) = 0;
        };
    }

}
#endif
