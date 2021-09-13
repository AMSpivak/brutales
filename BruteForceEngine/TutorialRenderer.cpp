#include "TutorialRenderer.h"
#include "Helpers.h"
#include "IndexedGeometryGenerator.h"


bool TutorialRenderer::LoadContent(BruteForce::Device& device)
{
    BruteForce::DataBlob vertexShaderBlob;
    ThrowIfFailed(D3DReadFileToBlob(L"BasicVertexShader.cso", &vertexShaderBlob));

    BruteForce::DataBlob pixelShaderBlob;
    ThrowIfFailed(D3DReadFileToBlob(L"BasicPixelShader.cso", &pixelShaderBlob));

    BruteForce::Geometry::CreateCube(device, m_cube);

    BruteForce::DescriptorHeapDesc dsvHeapDesc = {};
    dsvHeapDesc.NumDescriptors = 1;
    dsvHeapDesc.Type = BruteForce::DescriptorHeapDSV;
    dsvHeapDesc.Flags = BruteForce::DescriptorHeapFlagsNone;
    ThrowIfFailed(device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_DSVHeap)));

    return true;
}

void TutorialRenderer::Resize(BruteForce::Device& device, int width, int height)
{
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
        BruteForce::ResourceDesc rd = BruteForce::ResourceDesc::Tex2D(optimizedClearValue.Format, width, height,
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
