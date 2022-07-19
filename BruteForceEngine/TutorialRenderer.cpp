#include "TutorialRenderer.h"
#include "Helpers.h"
#include "IndexedGeometryGenerator.h"
#include "RenderInstanced.h"
#include "RenderTerrain.h"
#include "CalcTerrainShadow.h"

#include <DirectXMath.h>
constexpr BruteForce::TargetFormat render_format = BruteForce::TargetFormat_R16G16B16A16_Float;
constexpr BruteForce::TargetFormat output_format = BruteForce::TargetFormat_R8G8B8A8_Unorm;

TutorialRenderer::TutorialRenderer(BruteForce::Device& device,
    BruteForce::Window* pWindow,
    bool UseWarp,
    BruteForce::DescriptorHeapManager& SRV_Heap
)
    :MyRenderer(device, pWindow, UseWarp)
    , m_SRV_Heap(SRV_Heap)
    , m_CopyCommandQueue(device, BruteForce::CommandListTypeCopy)
    , m_time(0.0f)
    , m_ContentLoaded(false)
    , m_OutputFormat(output_format)
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
        HeapDesc.NumDescriptors = RendererNumFrames;
        HeapDesc.Type = BruteForce::DescriptorHeapRTV;
        HeapDesc.Flags = BruteForce::DescriptorHeapFlagsNone;
        ThrowIfFailed(device->CreateDescriptorHeap(&HeapDesc, IID_PPV_ARGS(&m_RTHeap)));
    }

    m_SRV_Heap.Create(device, 1000, BruteForce::DescriptorHeapCvbSrvUav);

    RTSrvDescriptors = m_SRV_Heap.AllocateManagedRange(device, static_cast<UINT>(RendererNumFrames), BruteForce::DescriptorRangeTypeSrv, "RenderTargetsSrvs");
    m_RenderSystems.push_back(std::make_shared<BruteForce::Render::RenderTerrain>());
    //m_RenderSystems.push_back(std::make_shared<BruteForce::Render::RenderInstanced>());
    m_CalcSystems.push_back(std::make_shared<BruteForce::Render::CalcTerrainShadow>());

    m_Camera.SetPosition({0.0f, 3.0f, -10.0f}, false);
    m_Camera.RecalculateView({ 0.0f, 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f, 0.0f });
    m_Camera.MoveView(0, 0, 0);

}

bool TutorialRenderer::LoadContent(BruteForce::Device& device)
{
    

    BruteForce::Render::RenderSubsystemInitDesc desc = {
                                                            render_format,
                                                            BruteForce::TargetFormat_D32_Float
                                                        };

    for (auto& subsystem : m_RenderSystems)
    {
        subsystem->LoadContent(device, m_NumFrames, desc, m_CopyCommandQueue, m_SRV_Heap);
    }

    for (auto& subsystem : m_CalcSystems)
    {
        subsystem->LoadContent(device, m_NumFrames);
    }

    BruteForce::Render::RenderSubsystemInitDesc desc_rt = {
                                                            m_OutputFormat,
                                                            BruteForce::TargetFormat_D32_Float
    };

    m_ToneMapper.SetRenderParameter(RTSrvDescriptors);
    m_ToneMapper.LoadContent(device, m_NumFrames, desc_rt, m_CopyCommandQueue, m_SRV_Heap);
    m_ContentLoaded = true;

    Resize(device);

    return true;
}

void TutorialRenderer::Update(float delta_time, BruteForce::SmartCommandQueue& compute_SmartCommandQueue)
{
    m_time += delta_time;
}


void TutorialRenderer::Resize(BruteForce::Device& device)
{
    int width = m_Window->GetWidth();
    int height = m_Window->GetHeight();
    m_Viewport = BruteForce::CreateViewport(0.0f, 0.0f,
        static_cast<float>(width), static_cast<float>(height));

    if (m_ContentLoaded)
    {
        width = std::max(1, width);
        height = std::max(1, height);
        m_Camera.SetAspectRatio(m_Window->GetWidth() / static_cast<float>(m_Window->GetHeight()), true);

        m_DepthBuffer.Assign(device, width, height, BruteForce::TargetFormat_D32_Float);
        BruteForce::DescriptorHandle d_handle = m_DSVHeap->GetCPUDescriptorHandleForHeapStart();
        m_DepthBuffer.CreateSrv(device, d_handle);

        BruteForce::DescriptorHandle rt_handle = m_RTHeap->GetCPUDescriptorHandleForHeapStart();
        BruteForce::DescriptorHandle srv_handle = RTSrvDescriptors->m_CpuHandle;

        /*for (uint8_t i = 0; i < RendererNumFrames; i++)
        {*/
            //m_RTTextures[0].Assign(device, width, height, render_format);
            //m_RTTextures[0].CreateViews(device, srv_handle, rt_handle);
        BruteForce::Textures::TexMetadata metadata;
        metadata.format = render_format;
        metadata.width = width;
        metadata.height = height;
        BruteForce::Textures::CreateTexture(m_RTTextures[0], metadata, device, true);
        m_RTTextures[0].CreateSrv(device, srv_handle);
        m_RTTextures[0].CreateRtv(device, rt_handle);

        /*    rt_handle.ptr += device->GetDescriptorHandleIncrementSize(BruteForce::DescriptorHeapRTV);
            srv_handle.ptr += device->GetDescriptorHandleIncrementSize(BruteForce::DescriptorHeapCvbSrvUav);
        }*/
    }
}


void TutorialRenderer::Render(BruteForce::SmartCommandQueue& in_SmartCommandQueue)
{
    auto smart_compute_command_list = m_ComputeSmartCommandQueue.GetCommandList();

    for (auto& subsystem : m_CalcSystems)
    {
        subsystem->PrepareRenderCommandList(smart_compute_command_list);
        //m_ComputeSmartCommandQueue
    }
    m_ComputeSmartCommandQueue.ExecuteCommandList(smart_compute_command_list);

    std::vector<BruteForce::SmartCommandList> command_lists;

    

    BruteForce::CDescriptorHandle rtv(m_BackBuffersDHeap->GetCPUDescriptorHandleForHeapStart(), m_CurrentBackBufferIndex, m_RTVDescriptorSize);
    BruteForce::DescriptorHandle dsv = m_DSVHeap->GetCPUDescriptorHandleForHeapStart();

    FLOAT clearColor[] = { 1.0f, 0.6f, 0.1f, 1.0f };

    auto& smart_command_list = command_lists.emplace_back(in_SmartCommandQueue.GetCommandList());
    {
        auto& commandList = smart_command_list.command_list;
        smart_command_list.ClearRTV(rtv, clearColor);
        smart_command_list.ClearDSV(dsv, true, false, 1.0f, 0);
    }
    
    auto& SetRT_cl = command_lists.emplace_back(in_SmartCommandQueue.GetCommandList());
    m_RTTextures[0].TransitionTo(SetRT_cl, BruteForce::ResourceStatesRenderTarget);
    SetRT_cl.ClearRTV(m_RTTextures[0].GetRT(), clearColor);

    BruteForce::Render::PrepareRenderHelper render_dest{
        &m_Viewport,
        &m_ScissorRect,
        &m_RTTextures[0].GetRT(),
        &dsv,
        m_Camera,
        static_cast<uint8_t>(m_CurrentBackBufferIndex),
        m_SRV_Heap
    };

    for (auto& subsystem : m_RenderSystems)
    {
        auto& list = command_lists.emplace_back(in_SmartCommandQueue.GetCommandList());
        subsystem->PrepareRenderCommandList(list, render_dest);
    }

    auto& ResetRT_cl = command_lists.emplace_back(in_SmartCommandQueue.GetCommandList());
    m_RTTextures[0].TransitionTo(ResetRT_cl, BruteForce::ResourceStatePixelShader);

    BruteForce::Render::PrepareRenderHelper render_dest_rt{
        &m_Viewport,
        &m_ScissorRect,
        &rtv,
        &dsv,
        m_Camera,
        static_cast<uint8_t>(m_CurrentBackBufferIndex),
        m_SRV_Heap
    };

    auto& ToneMap_cl = command_lists.emplace_back(in_SmartCommandQueue.GetCommandList());
    m_ToneMapper.PrepareRenderCommandList(ToneMap_cl, render_dest_rt);

    for (auto& execute_list : command_lists)
    {
        SetCurrentFence(in_SmartCommandQueue.ExecuteCommandList(execute_list));
    }
}

BruteForce::Camera* TutorialRenderer::GetCameraPtr()
{
    return &m_Camera;
}

TutorialRenderer::~TutorialRenderer()
{
    Flush();
}

