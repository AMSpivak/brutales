#include "TutorialRenderer.h"
#include "Helpers.h"
#include "IndexedGeometryGenerator.h"
#include "RenderInstanced.h"
#include "RenderTerrain.h"
#include "ScreenSpaceSky.h"
#include "ComputeTerrainShadow.h"

#include <DirectXMath.h>


void TutorialRenderer::CreateCommonResources(BruteForce::Device& device)
{
    RTSrvDescriptors = m_SRV_Heap.AllocateManagedRange(device, static_cast<UINT>(RenderNumFrames), BruteForce::DescriptorRangeTypeSrv, "RenderTargetsSrvs");
    RTNoScreenSrvDescriptors = m_SRV_Heap.AllocateManagedRange(device, static_cast<UINT>(NoScreenTextures), BruteForce::DescriptorRangeTypeSrv, "NoScreenRenderTargetsSrvs");

    SunShadowSrvDescriptors = m_SRV_Heap.AllocateManagedRange(device, static_cast<UINT>(SwapchainNumFrames), BruteForce::DescriptorRangeTypeSrv, "TerrainShadowSrvs");
    SunShadowUavDescriptors = m_SRV_Heap.AllocateManagedRange(device, static_cast<UINT>(SwapchainNumFrames), BruteForce::DescriptorRangeTypeUav, "TerrainShadowUavs");
    HeightmapTexturesRange = m_SRV_Heap.AllocateManagedRange(device, static_cast<UINT>(2), BruteForce::DescriptorRangeTypeSrv, "TerrainHeightmapTextures");

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
        BruteForce::Textures::CreateTexture(m_ShadowTextures[i], metadata, device, false, true);
        m_ShadowTextures[i].CreateSrv(device, *SunShadowSrvDescriptors, i);
        m_ShadowTextures[i].CreateUav(device, *SunShadowUavDescriptors, i);
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
        HeapDesc.NumDescriptors = RenderNumFrames + NoScreenTextures;
        HeapDesc.Type = BruteForce::DescriptorHeapRTV;
        HeapDesc.Flags = BruteForce::DescriptorHeapFlagsNone;
        ThrowIfFailed(device->CreateDescriptorHeap(&HeapDesc, IID_PPV_ARGS(&m_RTHeap)));
    }

    m_SRV_Heap.Create(device, 1000, BruteForce::DescriptorHeapCvbSrvUav);
    constexpr UINT sun_shadows = 1;
    //SunShadowSrvDescriptors = m_SRV_Heap.AllocateManagedRange(device, sun_shadows, BruteForce::DescriptorRangeTypeSrv, "SunShadowsSrvs");
    //SunShadowUavDescriptors = m_SRV_Heap.AllocateManagedRange(device, sun_shadows, BruteForce::DescriptorRangeTypeUav, "SunShadowsUavs");
    m_RenderSystems.push_back(std::make_shared<BruteForce::Render::ScreenSpaceSky>());

    m_RenderSystems.push_back(std::make_shared<BruteForce::Render::RenderTerrain>());
    //m_RenderSystems.push_back(std::make_shared<BruteForce::Render::RenderInstanced>());
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
    MyRenderer::Resize();

    int width = m_Window->GetWidth();
    int height = m_Window->GetHeight();
    
    m_Viewport = BruteForce::CreateViewport(0.0f, 0.0f,
        static_cast<float>(width), static_cast<float>(height));

    if (m_ContentLoaded)
    {
        width = std::max(1, width);
        height = std::max(1, height);
        m_Camera.SetAspectRatio(m_Window->GetWidth() / static_cast<float>(m_Window->GetHeight()), true);

        m_DepthBuffer.Assign(m_Device, width, height, BruteForce::TargetFormat_D32_Float);
        BruteForce::DescriptorHandle d_handle = m_DSVHeap->GetCPUDescriptorHandleForHeapStart();
        m_DepthBuffer.CreateSrv(m_Device, d_handle);

        BruteForce::DescriptorHandle rt_handle = m_RTHeap->GetCPUDescriptorHandleForHeapStart();
        BruteForce::DescriptorHandle srv_handle = RTSrvDescriptors->m_CpuHandle;

        BruteForce::Textures::TexMetadata metadata;
        metadata.format = render_format;
        metadata.width = width;
        metadata.height = height;
        for (int i = 0; i < RenderNumFrames; i++)
        {
            BruteForce::Textures::CreateTexture(m_RTTextures[i], metadata, m_Device, true, false);
            m_RTTextures[i].CreateSrv(m_Device, srv_handle);
            srv_handle.ptr += m_Device->GetDescriptorHandleIncrementSize(BruteForce::DescriptorHeapCvbSrvUav);
            m_RTTextures[i].CreateRtv(m_Device, rt_handle);
            rt_handle.ptr += m_Device->GetDescriptorHandleIncrementSize(BruteForce::DescriptorHeapRTV);
        }

        {
            {

                metadata.format = render_normals_format;
                int rt = RT(enRenderTargets::TBN_Quaternion);
                srv_handle = RTNoScreenSrvDescriptors->m_CpuHandle;
                srv_handle.ptr += rt * m_Device->GetDescriptorHandleIncrementSize(BruteForce::DescriptorHeapCvbSrvUav);

                BruteForce::Textures::CreateTexture(m_RTNoScreenTextures[rt], metadata, m_Device, true, false);
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

                BruteForce::Textures::CreateTexture(m_RTNoScreenTextures[rt], metadata, m_Device, true, false);
                m_RTNoScreenTextures[rt].CreateSrv(m_Device, srv_handle);
                srv_handle.ptr += m_Device->GetDescriptorHandleIncrementSize(BruteForce::DescriptorHeapCvbSrvUav);
                m_RTNoScreenTextures[rt].CreateRtv(m_Device, rt_handle);
                rt_handle.ptr += m_Device->GetDescriptorHandleIncrementSize(BruteForce::DescriptorHeapRTV);
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
        m_Camera.SetJitter(0.0f, (m_CurrentBackBufferIndex & 1)? jy: -jy, true);
        m_Camera.RecalculateMatrixes();
    }

    {
        auto smart_compute_command_list = m_ComputeSmartCommandQueue.GetCommandList();

        BruteForce::Compute::PrepareComputeHelper c_helper {
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
        m_ComputeSmartCommandQueue.ExecuteCommandList(smart_compute_command_list);
    }
    std::vector<BruteForce::SmartCommandList> command_lists;

    

    BruteForce::CDescriptorHandle rtv(m_BackBuffersDHeap->GetCPUDescriptorHandleForHeapStart(), m_CurrentBackBufferIndex, m_RTVDescriptorSize);
    BruteForce::DescriptorHandle dsv = m_DSVHeap->GetCPUDescriptorHandleForHeapStart();

    FLOAT clearColor[] = { 1.0f, 0.6f, 0.1f, 1.0f };
    FLOAT clearEmptyColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
        
    auto& SetRT_cl = command_lists.emplace_back(in_SmartCommandQueue.GetCommandList());
    SetRT_cl.BeginEvent(0, "Render to HDR RT");
    m_RTTextures[m_rt_index].TransitionTo(SetRT_cl, BruteForce::ResourceStatesRenderTarget);
    m_RTNoScreenTextures[RT(enRenderTargets::TBN_Quaternion)].TransitionTo(SetRT_cl, BruteForce::ResourceStatesRenderTarget);
    m_RTNoScreenTextures[RT(enRenderTargets::Materials)].TransitionTo(SetRT_cl, BruteForce::ResourceStatesRenderTarget);
    //SetRT_cl.ClearRTV(m_RTTextures[0].GetRT(), clearColor);
    SetRT_cl.ClearRTV(m_RTNoScreenTextures[RT(enRenderTargets::TBN_Quaternion)].GetRT(), clearEmptyColor);
    SetRT_cl.ClearRTV(m_RTNoScreenTextures[RT(enRenderTargets::Materials)].GetRT(), clearEmptyColor);
    SetRT_cl.ClearDSV(dsv, true, false, 1.0f, 0);

    const BruteForce::DescriptorHandle HdrRts[3] = {
        m_RTTextures[m_rt_index].GetRT()
        , m_RTNoScreenTextures[RT(enRenderTargets::TBN_Quaternion)].GetRT()
        , m_RTNoScreenTextures[RT(enRenderTargets::Materials)].GetRT()
    };

    BruteForce::Render::PrepareRenderHelper render_dest{
        &m_Viewport,
        &m_ScissorRect,
        HdrRts,
        3,
        &dsv,
        m_Camera,
        static_cast<uint8_t>(m_CurrentBackBufferIndex),
        m_rt_index,
        m_SRV_Heap
    };

    for (auto& subsystem : m_RenderSystems)
    {
        auto& list = command_lists.emplace_back(in_SmartCommandQueue.GetCommandList());
        subsystem->PrepareRenderCommandList(list, render_dest);
    }

    auto& ResetRT_cl = command_lists.emplace_back(in_SmartCommandQueue.GetCommandList());
    m_RTTextures[m_rt_index].TransitionTo(ResetRT_cl, BruteForce::ResourceStatePixelShader);
    m_RTNoScreenTextures[RT(enRenderTargets::TBN_Quaternion)].TransitionTo(ResetRT_cl, BruteForce::ResourceStatePixelShader);
    m_RTNoScreenTextures[RT(enRenderTargets::Materials)].TransitionTo(ResetRT_cl, BruteForce::ResourceStatePixelShader);
    ResetRT_cl.EndEvent();

    BruteForce::Render::PrepareRenderHelper render_dest_rt{
        &m_Viewport,
        &m_ScissorRect,
        &rtv,
        1,
        &dsv,
        m_Camera,
        static_cast<uint8_t>(m_CurrentBackBufferIndex),
        m_rt_index,
        m_SRV_Heap
    };

    auto& ToneMap_cl = command_lists.emplace_back(in_SmartCommandQueue.GetCommandList());
    m_ToneMapper.PrepareRenderCommandList(ToneMap_cl, render_dest_rt);

    for (auto& execute_list : command_lists)
    {
        SetCurrentFence(in_SmartCommandQueue.ExecuteCommandList(execute_list));
    }

    m_rt_index = (++m_rt_index) % RenderNumFrames;
}

BruteForce::Camera* TutorialRenderer::GetCameraPtr()
{
    return &m_Camera;
}

TutorialRenderer::~TutorialRenderer()
{
    Flush();
}

