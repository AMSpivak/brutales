#include "TutorialRenderer.h"
#include "Helpers.h"
#include "PlatformDefine.h"
#include "IndexedGeometryGenerator.h"
#include "RenderInstanced.h"
#include "RenderTerrain.h"
#include "ComputeTerrainShadow.h"
#include "BruteForceMath.h"
#include "GameEnvironment.h"
#include <DirectXMath.h>


void TutorialRenderer::CreateCommonResources(BruteForce::Device& device)
{
    RTSrvDescriptors = m_SRV_Heap.AllocateManagedRange(device, static_cast<UINT>(RenderNumFrames), BruteForce::DescriptorRangeTypeSrv, "RenderTargetsSrvs");
    RTUavDescriptors = m_SRV_Heap.AllocateManagedRange(device, static_cast<UINT>(RenderNumFrames), BruteForce::DescriptorRangeTypeUav, "RenderTargetsUavs");
    RTNoScreenSrvDescriptors = m_SRV_Heap.AllocateManagedRange(device, static_cast<UINT>(NoScreenTextures), BruteForce::DescriptorRangeTypeSrv, "NoScreenRenderTargetsSrvs");
    RTLuminanceSrvDescriptors = m_SRV_Heap.AllocateManagedRange(device, static_cast<UINT>(RenderNumFrames), BruteForce::DescriptorRangeTypeSrv, "RTLuminanceSrvs");
    //RTSrvUavDescriptors = m_SRV_Heap.AllocateManagedRange(device, static_cast<UINT>(RenderNumFrames), BruteForce::DescriptorRangeTypeSrv, "RenderTargetsUavs");

    SunShadowSrvDescriptors = m_SRV_Heap.AllocateManagedRange(device, static_cast<UINT>(SwapchainNumFrames), BruteForce::DescriptorRangeTypeSrv, "TerrainShadowSrvs");
    SunShadowUavDescriptors = m_SRV_Heap.AllocateManagedRange(device, static_cast<UINT>(SwapchainNumFrames), BruteForce::DescriptorRangeTypeUav, "TerrainShadowUavs");
    HeightmapTexturesRange = m_SRV_Heap.AllocateManagedRange(device, static_cast<UINT>(2), BruteForce::DescriptorRangeTypeSrv, "TerrainHeightmapTextures");
    LuminanceUavDescriptors = m_SRV_Heap.AllocateManagedRange(device, static_cast<UINT>(RenderNumFrames * 2), BruteForce::DescriptorRangeTypeUav, "LuminanceUavs");
    LuminanceSrvDescriptors = m_SRV_Heap.AllocateManagedRange(device, static_cast<UINT>(RenderNumFrames * 2), BruteForce::DescriptorRangeTypeSrv, "LuminanceSrvs");

    DepthSrvDescriptors = m_SRV_Heap.AllocateManagedRange(device, static_cast<UINT>(1), BruteForce::DescriptorRangeTypeSrv, "DepthSrvs");

    BruteForce::Textures::TexMetadata metadata;
    const int shadowsize = BruteForce::Compute::ComputeTerrainShadow::GetTerrainShadowSize();
    metadata.width = shadowsize;
    metadata.height = shadowsize;
    metadata.arraySize = 1;
    metadata.depth = 1;
    metadata.format = render_format;
    metadata.dimension = DirectX::TEX_DIMENSION_TEXTURE2D;
    metadata.mipLevels = 1;
    for (size_t i = 0; i < SwapchainNumFrames; i++)
    {
        BruteForce::Textures::CreateTexture(m_ShadowTextures[i], metadata, device, false, true, false);
        m_ShadowTextures[i].CreateSrv(device, *SunShadowSrvDescriptors, i);
        m_ShadowTextures[i].CreateUav(device, *SunShadowUavDescriptors, i);
    }

    //for (size_t i = 0; i < RenderNumFrames; i++)
    {
        size_t i = 0;
        metadata.format = render_luminance_format;
        metadata.width = 32;
        metadata.height = 32;
        BruteForce::Textures::CreateTexture(m_UAVLuminanceTextures[0], metadata, device, false, true, false);
        m_UAVLuminanceTextures[0].m_GpuBuffer->SetName(L"LuminanceLog32");
        m_UAVLuminanceTextures[0].CreateUav(device, *LuminanceUavDescriptors, i);
        m_UAVLuminanceTextures[0].CreateSrv(device, *LuminanceSrvDescriptors, i);


        metadata.width = 1;
        metadata.height = 1;
        BruteForce::Textures::CreateTexture(m_UAVLuminanceTextures[1], metadata, device, false, true, false);
        m_UAVLuminanceTextures[1].m_GpuBuffer->SetName(L"LuminanceLog1");
        m_UAVLuminanceTextures[1].CreateUav(device, *LuminanceUavDescriptors, i + 1);
        m_UAVLuminanceTextures[1].CreateSrv(device, *LuminanceSrvDescriptors, i + 1);

    }

}

TutorialRenderer::TutorialRenderer(BruteForce::Device& device, BruteForce::Adapter& adapter,
    BruteForce::Window* pWindow,
    bool UseWarp, BruteForce::TargetFormat t_format,
    BruteForce::DescriptorHeapManager& SRV_Heap
)
    :MyRenderer(device, adapter,  pWindow, UseWarp, t_format)
    , m_rt_index{ 0 }
    , m_SRV_Heap(SRV_Heap)
    , m_CopyCommandQueue(device, BruteForce::CommandListTypeCopy)
    , m_time(0.0f)
    , m_ContentLoaded(false)
    , m_fence_sky_shadow(device)
    , m_fence_avg_luminance(device)
    , m_fence_frame_luminance(device)
    , m_fence_gbuffer(device)
    , m_fence_deffered(device)
{
    //BruteForce::ReportLiveObjects();

    m_ScissorRect = BruteForce::ScissorRect{ 0, 0, LONG_MAX, LONG_MAX };
    m_Viewport = BruteForce::Viewport{ 0.0f, 0.0f, static_cast<float>(pWindow->GetWidth()), static_cast<float>(pWindow->GetHeight()) };
    
    {
        BruteForce::DescriptorHeapDesc dsvHeapDesc = {};
        dsvHeapDesc.NumDescriptors = 1;
        dsvHeapDesc.Type = BruteForce::DescriptorHeapDSV;
        dsvHeapDesc.Flags = BruteForce::DescriptorHeapFlagsNone;
        ThrowIfFailed(device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_DSVHeap)));
    }

    {
        BruteForce::DescriptorHeapDesc HeapDesc = {};
        HeapDesc.NumDescriptors = RenderNumFrames + NoScreenTextures + RenderNumFrames/*luminance*/;
        HeapDesc.Type = BruteForce::DescriptorHeapRTV;
        HeapDesc.Flags = BruteForce::DescriptorHeapFlagsNone;
        ThrowIfFailed(device->CreateDescriptorHeap(&HeapDesc, IID_PPV_ARGS(&m_RTHeap)));
    }

    m_SRV_Heap.Create(device, 1000, BruteForce::DescriptorHeapCvbSrvUav);
    constexpr UINT sun_shadows = 1;
    //SunShadowSrvDescriptors = m_SRV_Heap.AllocateManagedRange(device, sun_shadows, BruteForce::DescriptorRangeTypeSrv, "SunShadowsSrvs");
    //SunShadowUavDescriptors = m_SRV_Heap.AllocateManagedRange(device, sun_shadows, BruteForce::DescriptorRangeTypeUav, "SunShadowsUavs");
    //m_RenderSystems.push_back(std::make_shared<BruteForce::Render::ScreenSpaceSky>());

    m_RenderSystems.push_back(std::make_shared<BruteForce::Render::RenderTerrain>());
    m_RenderSystems.push_back(std::make_shared<BruteForce::Render::RenderInstanced>());
    m_CalcSystems.push_back(std::make_shared<BruteForce::Compute::ComputeTerrainShadow>());

    m_Camera.SetPosition({0.0f, 3.0f, -10.0f}, false);
    m_Camera.RecalculateView({ 0.0f, 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f, 0.0f });
    m_Camera.MoveView(0, 0, 0);

}

bool TutorialRenderer::LoadContent(BruteForce::Device& device)
{
    CreateCommonResources(device);

    BruteForce::Render::RenderSubsystemInitDesc desc = {
                                                            render_format,
                                                            BruteForce::TargetFormat_D32_Float,
                                                            nullptr
                                                        };
    desc.gpu_allocator_ptr = m_GpuAllocator;

    for (auto& subsystem : m_RenderSystems)
    {
        subsystem->LoadContent(device, m_NumFrames, desc, m_CopyCommandQueue, m_SRV_Heap);
    }

    m_SkyRender.LoadContent(device, m_NumFrames, desc, m_CopyCommandQueue, m_SRV_Heap);

    for (auto& subsystem : m_CalcSystems)
    {
        subsystem->LoadContent(device, m_NumFrames, m_SRV_Heap);
    }

    BruteForce::Render::RenderSubsystemInitDesc desc_rt = {
                                                            m_TargetFormat,
                                                            BruteForce::TargetFormat_D32_Float,
                                                            nullptr
    };

    m_ToneMapper.LoadContent(device, m_NumFrames, desc_rt, m_CopyCommandQueue, m_SRV_Heap);

    BruteForce::Render::RenderSubsystemInitDesc desc_lum = {
                                                        render_luminance_format,
                                                        BruteForce::TargetFormat_D32_Float,
                                                        nullptr
    };

    desc_lum.gpu_allocator_ptr = m_GpuAllocator;
    m_Luminance.LoadContent(device, m_NumFrames, desc_lum, m_CopyCommandQueue, m_SRV_Heap);



    m_CalculateLuminance.LoadContent(device, m_NumFrames, m_SRV_Heap);
    m_ComputeDeffered.LoadContent(device, m_NumFrames, m_SRV_Heap);

    m_ContentLoaded = true;

    Resize();

    return true;
}

void TutorialRenderer::Update(float delta_time, BruteForce::SmartCommandQueue& compute_SmartCommandQueue)
{
    m_time += delta_time;
}


void TutorialRenderer::Resize()
{

    //auto signal = m_SmartCommandQueue.Signal(m_fence_sky_shadow);
    //m_SmartCommandQueue.WaitForFenceValue(signal);
    MyRenderer::Resize();

    //bool can_hdr = m_Window->IsOnHDRDisplay(m_Adapter);
    //{
    //    char buffer[500];
    //    sprintf_s(buffer, 500, can_hdr ? "HDR is supported\n" : "No HDR support\n");
    //}

    int width = m_Window->GetWidth();
    int height = m_Window->GetHeight();
    
    m_Viewport = BruteForce::CreateViewport(0.0f, 0.0f,
        static_cast<float>(width), static_cast<float>(height));

    if (m_ContentLoaded)
    {
        width = std::max(1, width);
        height = std::max(1, height);
        m_Camera.SetAspectRatio(m_Window->GetWidth() / static_cast<float>(m_Window->GetHeight()), true);

        //m_DepthBuffer.Assign(m_Device, width, height, BruteForce::TargetFormat_D32_Float);
        //BruteForce::DescriptorHandle d_handle = m_DSVHeap->GetCPUDescriptorHandleForHeapStart();
        //m_DepthBuffer.CreateSrv(m_Device, d_handle);

        
        BruteForce::DescriptorHandle rt_handle = m_RTHeap->GetCPUDescriptorHandleForHeapStart();
        BruteForce::DescriptorHandle srv_handle = RTSrvDescriptors->m_CpuHandle;

        BruteForce::Textures::TexMetadata metadata;
        metadata.format = depth_format;
        metadata.width = width;
        metadata.height = height;

        {
            BruteForce::Textures::CreateTexture(m_DepthTexture, metadata, m_Device, false, false, true);
            BruteForce::DescriptorHandle d_handle = m_DSVHeap->GetCPUDescriptorHandleForHeapStart();
            m_DepthTexture.CreateDsv(m_Device, d_handle);
            BruteForce::DescriptorHandle depth_srv_handle = DepthSrvDescriptors->m_CpuHandle;
            m_DepthTexture.CreateSrv(m_Device, depth_srv_handle);
            m_DepthTexture.SetName(L"DepthBuffer");
            //m_DepthTexture
        }



        metadata.format = render_format;
        for (int i = 0; i < RenderNumFrames; i++)
        {
            BruteForce::Textures::CreateTexture(m_RTTextures[i], metadata, m_Device, true, true, false);
            //RTUavDescriptors
            m_RTTextures[i].CreateUav(m_Device, *RTUavDescriptors, i);
            m_RTTextures[i].CreateSrv(m_Device, srv_handle);
            srv_handle.ptr += m_Device->GetDescriptorHandleIncrementSize(BruteForce::DescriptorHeapCvbSrvUav);
            m_RTTextures[i].CreateRtv(m_Device, rt_handle);
            rt_handle.ptr += m_Device->GetDescriptorHandleIncrementSize(BruteForce::DescriptorHeapRTV);

            //m_RTTextures[i].CreateUav(m_Device, *RTSrvUavDescriptors, i);
        }

        {
            {

                metadata.format = render_normals_format;
                int rt = RT(enRenderTargets::TBN_Quaternion);
                srv_handle = RTNoScreenSrvDescriptors->m_CpuHandle;
                srv_handle.ptr += rt * m_Device->GetDescriptorHandleIncrementSize(BruteForce::DescriptorHeapCvbSrvUav);

                BruteForce::Textures::CreateTexture(m_RTNoScreenTextures[rt], metadata, m_Device, true, false, false);
                m_RTNoScreenTextures[rt].m_GpuBuffer->SetName(L"TBN_Quaternion");

                m_RTNoScreenTextures[rt].CreateSrv(m_Device, srv_handle);
                srv_handle.ptr += m_Device->GetDescriptorHandleIncrementSize(BruteForce::DescriptorHeapCvbSrvUav);
                m_RTNoScreenTextures[rt].CreateRtv(m_Device, rt_handle);
                rt_handle.ptr += m_Device->GetDescriptorHandleIncrementSize(BruteForce::DescriptorHeapRTV);
            }

            {
                metadata.format = render_materials_format;
                int rt = RT(enRenderTargets::Materials);
                srv_handle = RTNoScreenSrvDescriptors->m_CpuHandle;
                srv_handle.ptr += rt * m_Device->GetDescriptorHandleIncrementSize(BruteForce::DescriptorHeapCvbSrvUav);

                BruteForce::Textures::CreateTexture(m_RTNoScreenTextures[rt], metadata, m_Device, true, false, false);
                m_RTNoScreenTextures[rt].m_GpuBuffer->SetName(L"Materials");

                m_RTNoScreenTextures[rt].CreateSrv(m_Device, srv_handle);
                srv_handle.ptr += m_Device->GetDescriptorHandleIncrementSize(BruteForce::DescriptorHeapCvbSrvUav);
                m_RTNoScreenTextures[rt].CreateRtv(m_Device, rt_handle);
                rt_handle.ptr += m_Device->GetDescriptorHandleIncrementSize(BruteForce::DescriptorHeapRTV);
            }

            {
                metadata.format = render_uv_format;
                int rt = RT(enRenderTargets::TexUV);
                srv_handle = RTNoScreenSrvDescriptors->m_CpuHandle;
                srv_handle.ptr += rt * m_Device->GetDescriptorHandleIncrementSize(BruteForce::DescriptorHeapCvbSrvUav);

                BruteForce::Textures::CreateTexture(m_RTNoScreenTextures[rt], metadata, m_Device, true, false, false);
                m_RTNoScreenTextures[rt].m_GpuBuffer->SetName(L"TexUV");
                m_RTNoScreenTextures[rt].CreateSrv(m_Device, srv_handle);
                srv_handle.ptr += m_Device->GetDescriptorHandleIncrementSize(BruteForce::DescriptorHeapCvbSrvUav);
                m_RTNoScreenTextures[rt].CreateRtv(m_Device, rt_handle);
                rt_handle.ptr += m_Device->GetDescriptorHandleIncrementSize(BruteForce::DescriptorHeapRTV);
            }

            {
                metadata.format = render_uvddxddy_format;
                int rt = RT(enRenderTargets::TexDdxDdy);
                srv_handle = RTNoScreenSrvDescriptors->m_CpuHandle;
                srv_handle.ptr += rt * m_Device->GetDescriptorHandleIncrementSize(BruteForce::DescriptorHeapCvbSrvUav);

                BruteForce::Textures::CreateTexture(m_RTNoScreenTextures[rt], metadata, m_Device, true, false, false);
                m_RTNoScreenTextures[rt].m_GpuBuffer->SetName(L"TexDdxDdy");
                m_RTNoScreenTextures[rt].CreateSrv(m_Device, srv_handle);
                srv_handle.ptr += m_Device->GetDescriptorHandleIncrementSize(BruteForce::DescriptorHeapCvbSrvUav);
                m_RTNoScreenTextures[rt].CreateRtv(m_Device, rt_handle);
                rt_handle.ptr += m_Device->GetDescriptorHandleIncrementSize(BruteForce::DescriptorHeapRTV);
            }

            {
                int sidesize = width > height ? height : width;
                static constexpr int max_lum_buffer = 1024;
                if (sidesize >= max_lum_buffer)
                {
                    sidesize = max_lum_buffer;
                }
                else
                {
                    sidesize = static_cast<int>(BruteForce::Math::NearestLower2Pow(static_cast<unsigned int>(sidesize)));
                }
                sidesize = max_lum_buffer;
                metadata.format = render_luminance_format;
                metadata.width = sidesize;
                metadata.height = sidesize;
                //int rt = RT(enRenderTargets::TexDdxDdy);
                //srv_handle = RTNoScreenSrvDescriptors->m_CpuHandle;
                //srv_handle.ptr += rt * m_Device->GetDescriptorHandleIncrementSize(BruteForce::DescriptorHeapCvbSrvUav);
                for (int i = 0; i < RenderNumFrames; i++)
                {
                    BruteForce::Textures::CreateTexture(m_RTLuminanceTextures[i], metadata, m_Device, true, false, false);
                    m_RTLuminanceTextures[i].m_GpuBuffer->SetName(L"LuminanceLog");
                    m_RTLuminanceTextures[i].CreateSrv(m_Device, *RTLuminanceSrvDescriptors, i);
                    //srv_handle.ptr += m_Device->GetDescriptorHandleIncrementSize(BruteForce::DescriptorHeapCvbSrvUav);
                    m_RTLuminanceTextures[i].CreateRtv(m_Device, rt_handle);
                    rt_handle.ptr += m_Device->GetDescriptorHandleIncrementSize(BruteForce::DescriptorHeapRTV);
                }
            }

        }

        //m_rt_index = 0;
    }
}


void TutorialRenderer::Render(BruteForce::SmartCommandQueue& in_SmartCommandQueue)
{
    {
        int width = m_Window->GetWidth();
        int height = m_Window->GetHeight();
        float jx = 0.0f;// 0.5f / width;
        float jy = 0.0f;// 0.5f / height;
        m_Camera.SetJitter(0.0f, (m_CurrentBackBufferIndex & 1) ? jy : -jy, true);
        m_Camera.RecalculateMatrixes();
    }

    
    {
        auto smart_compute_command_list = m_ComputeSmartCommandQueue.GetCommandList();

        BruteForce::Compute::PrepareComputeHelper c_helper{
            &m_Viewport,
            &m_ScissorRect,
            m_Camera,
            static_cast<uint8_t>(m_CurrentBackBufferIndex),
            m_SRV_Heap };

        for (auto& subsystem : m_CalcSystems)
        {
            subsystem->PrepareRenderCommandList(smart_compute_command_list, c_helper);
            //m_ComputeSmartCommandQueue
        }
        //m_UAVLuminanceTextures[1].TransitionTo(smart_compute_command_list, BruteForce::ResourceStatesRenderTarget);
        auto& game_info = BruteForce::GlobalLevelInfo::ReadGlobalGameCameraInfo();
        if(game_info.m_EyeAdapt)
        {
            m_CalculateLuminance.PrepareRenderCommandList(smart_compute_command_list, c_helper);
        }
        //m_UAVLuminanceTextures[1].TransitionTo(smart_compute_command_list, BruteForce::ResourceStatesRenderTarget);

        m_ComputeSmartCommandQueue.ExecuteCommandList(smart_compute_command_list);
        //m_ComputeSmartCommandQueue.Signal(m_fence_sky_shadow);
        m_ComputeSmartCommandQueue.GpuWaitNext(m_fence_gbuffer);

        auto compute_deffered_command_list = m_ComputeSmartCommandQueue.GetCommandList();
        m_RTTextures[0].TransitionTo(compute_deffered_command_list, BruteForce::ResourceStateUav);
        m_RTTextures[1].TransitionTo(compute_deffered_command_list, BruteForce::ResourceStateUav);
        m_RTNoScreenTextures[RT(enRenderTargets::TBN_Quaternion)].TransitionTo(compute_deffered_command_list, BruteForce::ResourceStateNonPixel);
        m_RTNoScreenTextures[RT(enRenderTargets::Materials)].TransitionTo(compute_deffered_command_list, BruteForce::ResourceStateNonPixel);
        m_RTNoScreenTextures[RT(enRenderTargets::TexUV)].TransitionTo(compute_deffered_command_list, BruteForce::ResourceStateNonPixel);
        m_RTNoScreenTextures[RT(enRenderTargets::TexDdxDdy)].TransitionTo(compute_deffered_command_list, BruteForce::ResourceStateNonPixel);
        m_DepthTexture.TransitionTo(compute_deffered_command_list, BruteForce::ResourceStateNonPixel);
        //c_helper.frame_index = m_rt_index;
        BruteForce::Compute::PrepareComputeHelper deffered_helper{
            &m_Viewport,
            &m_ScissorRect,
            m_Camera,
            static_cast<uint8_t>(m_rt_index),
            m_SRV_Heap };

        m_ComputeDeffered.PrepareRenderCommandList(compute_deffered_command_list, deffered_helper);

        m_RTTextures[0].TransitionTo(compute_deffered_command_list, BruteForce::ResourceStateCommon);
        m_RTTextures[1].TransitionTo(compute_deffered_command_list, BruteForce::ResourceStateCommon);
        m_DepthTexture.TransitionTo(compute_deffered_command_list, BruteForce::ResourceStateCommon);

        m_ComputeSmartCommandQueue.ExecuteCommandList(compute_deffered_command_list);
        m_ComputeSmartCommandQueue.Signal(m_fence_deffered);
    }
    std::vector<BruteForce::SmartCommandList> command_lists;



    BruteForce::CDescriptorHandle rtv(m_BackBuffersDHeap->GetCPUDescriptorHandleForHeapStart(), m_CurrentBackBufferIndex, m_RTVDescriptorSize);
    BruteForce::DescriptorHandle dsv = m_DSVHeap->GetCPUDescriptorHandleForHeapStart();

    FLOAT clearColor[] = { 1.0f, 0.6f, 0.1f, 1.0f };
    FLOAT clearEmptyColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };

    auto& SetRT_cl = command_lists.emplace_back(in_SmartCommandQueue.GetCommandList());
    SetRT_cl.BeginEvent(0, "Render to HDR RT");
    m_DepthTexture.TransitionTo(SetRT_cl, BruteForce::ResourceStateDepthWrite);

    m_RTTextures[m_rt_index].TransitionTo(SetRT_cl, BruteForce::ResourceStatesRenderTarget);
    m_RTNoScreenTextures[RT(enRenderTargets::TBN_Quaternion)].TransitionTo(SetRT_cl, BruteForce::ResourceStatesRenderTarget);
    m_RTNoScreenTextures[RT(enRenderTargets::Materials)].TransitionTo(SetRT_cl, BruteForce::ResourceStatesRenderTarget);
    m_RTNoScreenTextures[RT(enRenderTargets::TexUV)].TransitionTo(SetRT_cl, BruteForce::ResourceStatesRenderTarget);
    m_RTNoScreenTextures[RT(enRenderTargets::TexDdxDdy)].TransitionTo(SetRT_cl, BruteForce::ResourceStatesRenderTarget);
    SetRT_cl.ClearRTV(m_RTTextures[m_rt_index].GetRT(), clearColor);
    SetRT_cl.ClearRTV(m_RTNoScreenTextures[RT(enRenderTargets::TBN_Quaternion)].GetRT(), clearEmptyColor);
    SetRT_cl.ClearRTV(m_RTNoScreenTextures[RT(enRenderTargets::Materials)].GetRT(), clearEmptyColor);
    SetRT_cl.ClearDSV(dsv, true, false, 1.0f, 0);

    const auto desc_rt = m_RTTextures[m_rt_index].GetRT();

    BruteForce::Render::PrepareRenderHelper sky_dest{
        &m_Viewport,
        &m_ScissorRect,
        &desc_rt,
        1,
        &dsv,
        m_Camera,
        static_cast<uint8_t>(m_CurrentBackBufferIndex),
        m_rt_index,
        m_Window->GetMaxNits(),
        m_SRV_Heap
    };
    m_SkyRender.PrepareRenderCommandList(SetRT_cl, sky_dest);
    SetRT_cl.BeginEvent(0, "Render to GBUFFER");

    const BruteForce::DescriptorHandle GbufferRts[NoScreenTextures] = {
         m_RTNoScreenTextures[RT(enRenderTargets::TBN_Quaternion)].GetRT()
        , m_RTNoScreenTextures[RT(enRenderTargets::Materials)].GetRT()
        , m_RTNoScreenTextures[RT(enRenderTargets::TexUV)].GetRT()
        , m_RTNoScreenTextures[RT(enRenderTargets::TexDdxDdy)].GetRT()
    };

    BruteForce::Render::PrepareRenderHelper render_dest{
        &m_Viewport,
        &m_ScissorRect,
        GbufferRts,
        NoScreenTextures,
        &dsv,
        m_Camera,
        static_cast<uint8_t>(m_CurrentBackBufferIndex),
        m_rt_index,
        m_Window->GetMaxNits(),
        m_SRV_Heap
    };

    
    
    constexpr bool separate_threads = false;

    if (separate_threads)
    {
        for (auto& subsystem : m_RenderSystems)
        {
            auto& list = command_lists.emplace_back(in_SmartCommandQueue.GetCommandList());
            subsystem->PrepareRenderCommandList(list, render_dest);
        }
    }
    else
    {
        auto& list = command_lists.emplace_back(in_SmartCommandQueue.GetCommandList());
        //for (auto& subsystem : m_RenderSystems)
        {
            auto& subsystem = m_RenderSystems[1];
            subsystem->PrepareRenderCommandList(list, render_dest);
        }
    
    }

    {
        auto& list = command_lists.emplace_back(in_SmartCommandQueue.GetCommandList());
        m_DepthTexture.TransitionTo(list, BruteForce::ResourceStateCommon);

        m_RTTextures[0].TransitionTo(list, BruteForce::ResourceStateCommon);
        m_RTTextures[1].TransitionTo(list, BruteForce::ResourceStateCommon);
        m_RTNoScreenTextures[RT(enRenderTargets::TBN_Quaternion)].TransitionTo(list, BruteForce::ResourceStateCommon);
        m_RTNoScreenTextures[RT(enRenderTargets::Materials)].TransitionTo(list, BruteForce::ResourceStateCommon);
        m_RTNoScreenTextures[RT(enRenderTargets::TexUV)].TransitionTo(list, BruteForce::ResourceStateCommon);
        m_RTNoScreenTextures[RT(enRenderTargets::TexDdxDdy)].TransitionTo(list, BruteForce::ResourceStateCommon);
        list.EndEvent();

    }

    

    auto ResetRT_cl = in_SmartCommandQueue.GetCommandList();
    ResetRT_cl.command_list->SetName(L"ResetRT");
    
    ResetRT_cl.EndEvent();

    
    {
        //in_SmartCommandQueue.GpuWait(m_fence_avg_luminance);
        int lum_index = 0;
        m_RTLuminanceTextures[lum_index].TransitionTo(ResetRT_cl, BruteForce::ResourceStatesRenderTarget);
        const BruteForce::DescriptorHandle LuminanceRts[1] = { m_RTLuminanceTextures[lum_index].GetRT() };
        BruteForce::Viewport vp{ 0,0, m_RTLuminanceTextures[lum_index].GetWidth(), m_RTLuminanceTextures[lum_index].GetHeight() };
        BruteForce::ScissorRect sr{ 0,0, m_RTLuminanceTextures[lum_index].GetWidth(), m_RTLuminanceTextures[lum_index].GetHeight() };

        BruteForce::Render::PrepareRenderHelper render_dest_lum{
            &vp,
            &sr,
            LuminanceRts,
            1,
            nullptr,
            m_Camera,
            static_cast<uint8_t>(m_CurrentBackBufferIndex),
            m_rt_index,
            m_Window->GetMaxNits(),
            m_SRV_Heap
        };

        //auto& ToneMap_cl = command_lists.emplace_back(in_SmartCommandQueue.GetCommandList());
        //ResetRT_cl
        m_Luminance.PrepareRenderCommandList(ResetRT_cl, render_dest_lum);
        m_RTLuminanceTextures[lum_index].TransitionTo(ResetRT_cl, BruteForce::ResourceStateCommon);
    }

    {
        //m_ComputeSmartCommandQueue.GpuWait(m_fence_frame_luminance);


        //m_ComputeSmartCommandQueue.Signal(m_fence_avg_luminance);
    }

    

    BruteForce::Render::PrepareRenderHelper render_dest_rt{
        &m_Viewport,
        &m_ScissorRect,
        &rtv,
        1,
        nullptr,
        m_Camera,
        static_cast<uint8_t>(m_CurrentBackBufferIndex),
        m_rt_index,
        m_Window->GetMaxNits(),
        m_SRV_Heap
    };

    
    m_ToneMapper.SetHDRMode(m_HDRmode);
    m_ToneMapper.PrepareRenderCommandList(ResetRT_cl, render_dest_rt);

    //uint64_t fence_value = 0;
    //in_SmartCommandQueue.GpuWait(m_fence_sky_shadow);
    for (auto& execute_list : command_lists)
    {
        //SetCurrentFenceValue(in_SmartCommandQueue.ExecuteCommandList(execute_list));
        
        in_SmartCommandQueue.ExecuteCommandList(execute_list);
    }
    in_SmartCommandQueue.Signal(m_fence_gbuffer);
    in_SmartCommandQueue.GpuWait(m_fence_deffered);
    in_SmartCommandQueue.ExecuteCommandList(ResetRT_cl);




    m_rt_index = (++m_rt_index) % RenderNumFrames;
}

BruteForce::Camera* TutorialRenderer::GetCameraPtr()
{
    return &m_Camera;
}

TutorialRenderer::~TutorialRenderer()
{
    //auto signal = m_SmartCommandQueue.Signal(m_fence_sky_shadow);
    //m_SmartCommandQueue.WaitForFenceValue(signal);
    Flush();
}

