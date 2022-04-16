#include "TutorialRenderer.h"
#include "Helpers.h"
#include "IndexedGeometryGenerator.h"
#include "RenderInstanced.h"
#include "RenderTerrain.h"
#include <DirectXMath.h>

TutorialRenderer::TutorialRenderer(BruteForce::Device& device, BruteForce::Window* pWindow, bool UseWarp) :MyRenderer(device, pWindow, UseWarp),
//, m_ScissorRect(ScissorRect(0, 0, LONG_MAX, LONG_MAX))
//, m_Viewport(Viewport(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height)))
m_CopyCommandQueue(device, BruteForce::CommandListTypeCopy)
, m_time(0.0f)
, m_ContentLoaded(false)
{
    //BruteForce::ReportLiveObjects();

    m_ScissorRect = BruteForce::ScissorRect{ 0, 0, LONG_MAX, LONG_MAX };
    m_Viewport = BruteForce::Viewport{ 0.0f, 0.0f, static_cast<float>(pWindow->GetWidth()), static_cast<float>(pWindow->GetHeight()) };

    BruteForce::DescriptorHeapDesc dsvHeapDesc = {};
    dsvHeapDesc.NumDescriptors = 1;
    dsvHeapDesc.Type = BruteForce::DescriptorHeapDSV;
    dsvHeapDesc.Flags = BruteForce::DescriptorHeapFlagsNone;
    ThrowIfFailed(device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_DSVHeap)));

    //m_RenderSystems.push_back(std::make_shared<BruteForce::Render::RenderInstanced>());
    m_RenderSystems.push_back(std::make_shared<BruteForce::Render::RenderTerrain>());

    m_Camera.SetPosition({0.0f, 3.0f, -10.0f}, false);
    m_Camera.RecalculateView({ 0.0f, 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f, 0.0f });
    m_Camera.MoveView(0, 0, 0);

}

bool TutorialRenderer::LoadContent(BruteForce::Device& device)
{
    for (auto& subsystem : m_RenderSystems)
    {
        subsystem->LoadContent(device, m_NumFrames);
    }
    m_ContentLoaded = true;

    Resize(device);

    return true;
}

void TutorialRenderer::Update(float delta_time)
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
        D3D12_CLEAR_VALUE optimizedClearValue = {};
        optimizedClearValue.Format = BruteForce::TargetFormat_D32_Float;
        optimizedClearValue.DepthStencil = { 1.0f, 0 };
        BruteForce::HeapProperties hp{ BruteForce::HeapTypeDefault };
        BruteForce::CResourceDesc rd = BruteForce::CResourceDesc::Tex2D(optimizedClearValue.Format, width, height,
                                        1, 0, 1, 0, BruteForce::ResourceFlagsDepthStencil);
        ThrowIfFailed(device->CreateCommittedResource(
            &hp,
            BruteForce::HeapFlagsNone,
            &rd,
            BruteForce::ResourceStateDepthWrite,
            &optimizedClearValue,
            IID_PPV_ARGS(&m_DepthBuffer)
        ));

        D3D12_DEPTH_STENCIL_VIEW_DESC dsv = {};
        dsv.Format = DXGI_FORMAT_D32_FLOAT;
        dsv.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
        dsv.Texture2D.MipSlice = 0;
        dsv.Flags = D3D12_DSV_FLAG_NONE;

        device->CreateDepthStencilView(m_DepthBuffer.Get(), &dsv,
            m_DSVHeap->GetCPUDescriptorHandleForHeapStart());
    }
}


void TutorialRenderer::Render(BruteForce::SmartCommandQueue& in_SmartCommandQueue)
{
    auto smart_command_list = in_SmartCommandQueue.GetCommandList();
    auto& commandList = smart_command_list.command_list;
    FLOAT clearColor[] = { 1.0f, 0.6f, 0.1f, 1.0f };

    BruteForce::CDescriptorHandle rtv(m_BackBuffersDHeap->GetCPUDescriptorHandleForHeapStart(), m_CurrentBackBufferIndex, m_RTVDescriptorSize);
    BruteForce::DescriptorHandle dsv = m_DSVHeap->GetCPUDescriptorHandleForHeapStart();
    smart_command_list.ClearRTV(rtv, clearColor);
    smart_command_list.ClearDSV(dsv, true, false, 1.0f, 0);
    SetCurrentFence(in_SmartCommandQueue.ExecuteCommandList(smart_command_list));
    
    std::vector<BruteForce::SmartCommandList> command_lists;

    BruteForce::Render::RenderDestination render_dest{
        &m_Viewport,
        &m_ScissorRect,
        &rtv,
        &dsv,
        m_Camera,
        static_cast<uint8_t>(m_CurrentBackBufferIndex)
    };

    for (auto& subsystem : m_RenderSystems)
    {
        auto& list = command_lists.emplace_back(in_SmartCommandQueue.GetCommandList());
        subsystem->PrepareRenderCommandList(list, render_dest);
    }

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

