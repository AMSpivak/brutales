#include "TutorialRenderer.h"
#include "Helpers.h"
#include "IndexedGeometryGenerator.h"
#include <DirectXMath.h>


TutorialRenderer::TutorialRenderer(BruteForce::Device& device, BruteForce::Window* pWindow, bool UseWarp) :MyRenderer(device, pWindow, UseWarp),
//, m_ScissorRect(ScissorRect(0, 0, LONG_MAX, LONG_MAX))
//, m_Viewport(Viewport(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height)))
m_CopyCommandQueue(device, BruteForce::CommandListTypeCopy)
, m_FoV(45.0)
, m_time(0.0f)
, m_ContentLoaded(false)
{
    m_ScissorRect = BruteForce::ScissorRect{ 0, 0, LONG_MAX, LONG_MAX };
    m_Viewport = BruteForce::Viewport{ 0.0f, 0.0f, static_cast<float>(pWindow->GetWidth()), static_cast<float>(pWindow->GetHeight()) };
    BruteForce::DescriptorHeapDesc descHeapCbvSrv = {};
    descHeapCbvSrv.NumDescriptors = 2;
    descHeapCbvSrv.Type = BruteForce::DescriptorHeapCvbSrvUav;
    descHeapCbvSrv.Flags = BruteForce::DescriptorHeapShaderVisible;
    ThrowIfFailed(device->CreateDescriptorHeap(&descHeapCbvSrv, __uuidof(ID3D12DescriptorHeap), (void**)&m_SVRHeap));
}

bool TutorialRenderer::LoadContent(BruteForce::Device& device)
{
    {
        D3D12_DESCRIPTOR_HEAP_DESC descHeapSampler = {};
        descHeapSampler.NumDescriptors = 1;
        descHeapSampler.Type = BruteForce::DescriptorHeapSampler;
        descHeapSampler.Flags = BruteForce::DescriptorHeapShaderVisible;
        ThrowIfFailed(device->CreateDescriptorHeap(&descHeapSampler, __uuidof(ID3D12DescriptorHeap), (void**)&m_SamplerHeap));

        BruteForce::SamplerDesc samplerDesc;
        ZeroMemory(&samplerDesc, sizeof(samplerDesc));
        samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
        samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        samplerDesc.MinLOD = 0;
        samplerDesc.MaxLOD = 0;// BruteForce::Math::floatMax;
        samplerDesc.MipLODBias = 0.0f;
        samplerDesc.MaxAnisotropy = 1;
        samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
        device->CreateSampler(&samplerDesc, m_SamplerHeap->GetCPUDescriptorHandleForHeapStart());
    }

    auto srv_handle = m_SVRHeap->GetCPUDescriptorHandleForHeapStart();

    BruteForce::Textures::LoadTextureFromFile(m_texture, L"test1.png", device, m_CopyCommandQueue, srv_handle);
    srv_handle.ptr += device->GetDescriptorHandleIncrementSize(BruteForce::DescriptorHeapCvbSrvUav);
    BruteForce::Textures::LoadTextureFromFile(m_texture_2, L"test3.png", device, m_CopyCommandQueue, srv_handle);

    BruteForce::DataBlob vertexShaderBlob;
    ThrowIfFailed(D3DReadFileToBlob(L"BasicVertexShader.cso", &vertexShaderBlob));

    BruteForce::DataBlob pixelShaderBlob;
    ThrowIfFailed(D3DReadFileToBlob(L"BasicPixelShader.cso", &pixelShaderBlob));

    BruteForce::DescriptorHeapDesc dsvHeapDesc = {};
    dsvHeapDesc.NumDescriptors = 1;
    dsvHeapDesc.Type = BruteForce::DescriptorHeapDSV;
    dsvHeapDesc.Flags = BruteForce::DescriptorHeapFlagsNone;
    ThrowIfFailed(device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_DSVHeap)));

    D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };

    D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
    featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
    if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
    {
        featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
    }

    D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
        D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;// |
        //D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;
    CD3DX12_DESCRIPTOR_RANGE1 descRange[2];
    descRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 2, 0);
    descRange[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0);
    CD3DX12_ROOT_PARAMETER1 rootParameters[3];
    rootParameters[2].InitAsConstants(sizeof(BruteForce::Math::Matrix) / 4, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);
    //rootParameters[3].InitAsConstants(sizeof(BruteForce::Math::Matrix) / 4, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);

    rootParameters[0].InitAsDescriptorTable(1, &descRange[0],  D3D12_SHADER_VISIBILITY_PIXEL);
    rootParameters[1].InitAsDescriptorTable(1, &descRange[1],  D3D12_SHADER_VISIBILITY_PIXEL);

    //CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDescription;
    //rootSignatureDescription.Init(3, rootParameters);
    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription;

    rootSignatureDescription.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr, rootSignatureFlags);

    BruteForce::DataBlob rootSignatureBlob;
    BruteForce::DataBlob errorBlob;
    ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDescription,
        featureData.HighestVersion, &rootSignatureBlob, &errorBlob));

    // Create the root signature.
    ThrowIfFailed(device->CreateRootSignature(0, rootSignatureBlob->GetBufferPointer(),
        rootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature)));

    
    struct PipelineStateStream
    {
        CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRootSignature;
        CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
        CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
        CD3DX12_PIPELINE_STATE_STREAM_VS VS;
        CD3DX12_PIPELINE_STATE_STREAM_PS PS;
        CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
        CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
    } pipelineStateStream;

    D3D12_RT_FORMAT_ARRAY rtvFormats = {};
    rtvFormats.NumRenderTargets = 1;
    rtvFormats.RTFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;

    pipelineStateStream.pRootSignature = m_RootSignature.Get();
    pipelineStateStream.InputLayout = { inputLayout, _countof(inputLayout) };
    pipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(vertexShaderBlob.Get());
    pipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(pixelShaderBlob.Get());
    pipelineStateStream.DSVFormat = DXGI_FORMAT_D32_FLOAT;
    pipelineStateStream.RTVFormats = rtvFormats;

    D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = {
        sizeof(PipelineStateStream), &pipelineStateStream
    };
    ThrowIfFailed(device->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&m_PipelineState)));

    BruteForce::Geometry::CreateCube(device, m_cube);

    m_ContentLoaded = true;

    Resize(device);

    return true;
}

void TutorialRenderer::Update(float delta_time)
{
    m_time += delta_time;
    float angle = static_cast<float>(m_time * 0.1f);
    const BruteForce::Math::Vector rotationAxis = BruteForce::Math::VectorSet(0, 1, 1, 0);
    m_ModelMatrix = BruteForce::Math::MatrixRotationAxis(rotationAxis, BruteForce::Math::DegToRad(angle));
    const BruteForce::Math::Vector eyePosition = BruteForce::Math::VectorSet(0, 0, -10, 1);
    const BruteForce::Math::Vector focusPoint = BruteForce::Math::VectorSet(0, 0, 0, 1);
    const BruteForce::Math::Vector upDirection = BruteForce::Math::VectorSet(0, 1, 0, 0);
    m_ViewMatrix = BruteForce::Math::MatrixLookAtLH(eyePosition, focusPoint, upDirection);

    float aspectRatio = m_Window->GetWidth() / static_cast<float>(m_Window->GetHeight());
    m_ProjectionMatrix = BruteForce::Math::MatrixPerspectiveFovLH(BruteForce::Math::DegToRad(m_FoV), aspectRatio, 0.1f, 100.0f);
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
    auto dsv = m_DSVHeap->GetCPUDescriptorHandleForHeapStart();
    smart_command_list.ClearRTV(rtv, clearColor);
    smart_command_list.ClearDSV(dsv, true, false, 1.0f, 0);
    smart_command_list.SetPipelineState(m_PipelineState);
    smart_command_list.SetRootSignature(m_RootSignature);

    /*BruteForce::ResourceBarrier barrier_dest = BruteForce::ResourceBarrier::Transition(
        m_texture.image.Get(),
        BruteForce::ResourceStateCommon,
        BruteForce::ResourceStatePixelShader);
    commandList->ResourceBarrier(1, &barrier_dest);*/


    ID3D12DescriptorHeap* ppHeaps[] = { m_SVRHeap.Get(), m_SamplerHeap.Get() };
    commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

    commandList->SetGraphicsRootDescriptorTable(0,
    m_SVRHeap->GetGPUDescriptorHandleForHeapStart());
    commandList->SetGraphicsRootDescriptorTable(1,
    m_SamplerHeap->GetGPUDescriptorHandleForHeapStart());
    //commandList->SetGraphicsRootSignature(m_RootSignature.Get());
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    commandList->IASetVertexBuffers(0, 1, &m_cube.m_VertexBufferView);
    commandList->IASetIndexBuffer(&m_cube.m_IndexBufferView);
    commandList->RSSetViewports(1, &m_Viewport);
    commandList->RSSetScissorRects(1, &m_ScissorRect);
    commandList->OMSetRenderTargets(1, &rtv, FALSE, &dsv);
    BruteForce::Math::Matrix mvpMatrix = DirectX::XMMatrixMultiply(m_ModelMatrix, m_ViewMatrix);
    mvpMatrix = DirectX::XMMatrixMultiply(mvpMatrix, m_ProjectionMatrix);
    auto offset = sizeof(BruteForce::Math::Matrix) / 4;
    commandList->SetGraphicsRoot32BitConstants(2, offset, &mvpMatrix, 0);
    //commandList->SetGraphicsRoot32BitConstants(3, sizeof()/4, &mvpMatrix, offset);


    commandList->DrawIndexedInstanced(m_cube.m_IndexesCount, 2, 0, 0, 0);

    SetCurrentFence(in_SmartCommandQueue.ExecuteCommandList(smart_command_list));

}
TutorialRenderer::~TutorialRenderer()
{
    Flush();
    m_SamplerHeap->Release();
    m_SVRHeap->Release();
    //ReportLiveObjects()

}

