#include "RenderTerrain.h"
#include "Helpers.h"
#include "IndexedGeometryGenerator.h"

namespace BruteForce
{
    namespace Render
    {
        RenderTerrain::RenderTerrain()
        {
        }
        RenderTerrain::~RenderTerrain()
        {
        }
        void RenderTerrain::Update(float delta_time)
        {
        }
        void RenderTerrain::LoadContent(Device& device)
        {
            {
                D3D12_DESCRIPTOR_HEAP_DESC descHeapSampler = {};
                descHeapSampler.NumDescriptors = 2;
                descHeapSampler.Type = BruteForce::DescriptorHeapSampler;
                descHeapSampler.Flags = BruteForce::DescriptorHeapShaderVisible;
                ThrowIfFailed(device->CreateDescriptorHeap(&descHeapSampler, __uuidof(ID3D12DescriptorHeap), (void**)&m_SamplerHeap));
                m_SamplerHeap->SetName(L"Sampler heap");

                BruteForce::SamplerDesc samplerDesc;
                ZeroMemory(&samplerDesc, sizeof(samplerDesc));
                samplerDesc.Filter = D3D12_FILTER_ANISOTROPIC;// D3D12_FILTER_MIN_MAG_MIP_LINEAR;
                samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
                samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
                samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
                samplerDesc.MinLOD = 0;
                samplerDesc.MaxLOD = 0;// BruteForce::Math::floatMax;
                samplerDesc.MipLODBias = 0.0f;
                samplerDesc.MaxAnisotropy = 1;
                samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
                auto sampler_handle = m_SamplerHeap->GetCPUDescriptorHandleForHeapStart();
                device->CreateSampler(&samplerDesc, sampler_handle);
                sampler_handle.ptr += device->GetDescriptorHandleIncrementSize(BruteForce::DescriptorHeapSampler);
                samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
                device->CreateSampler(&samplerDesc, sampler_handle);
                
                
            }
            {
                const std::wstring tex_names[] = { { L"desert_map.png"}, { L"Desert_Rock_albedo.png"}, {L"Desert_Sand_albedo.png"} };
                size_t textures_count = _countof(tex_names);

                BruteForce::DescriptorHeapDesc descHeapCbvSrv = {};
                descHeapCbvSrv.NumDescriptors = textures_count;
                descHeapCbvSrv.Type = BruteForce::DescriptorHeapCvbSrvUav;
                descHeapCbvSrv.Flags = BruteForce::DescriptorHeapShaderVisible;
                ThrowIfFailed(device->CreateDescriptorHeap(&descHeapCbvSrv, __uuidof(ID3D12DescriptorHeap), (void**)&m_SVRHeap));
                m_SVRHeap->SetName(L"Descriptor heap");

                auto srv_handle = m_SVRHeap->GetCPUDescriptorHandleForHeapStart();
                SmartCommandQueue m_CopyCommandQueue(device, BruteForce::CommandListTypeCopy);

                for (int i = 0; i < textures_count; i++)
                {
                    auto texture = m_textures.emplace_back(std::make_shared<BruteForce::Textures::Texture>());
                    BruteForce::Textures::LoadTextureFromFile(*texture, tex_names[i], device, m_CopyCommandQueue);
                    texture->image->SetName(L"Texture image");
                    texture->CreateSrv(device, srv_handle);
                    srv_handle.ptr += device->GetDescriptorHandleIncrementSize(BruteForce::DescriptorHeapCvbSrvUav);
                }
            }

            BruteForce::DataBlob vertexShaderBlob;
            ThrowIfFailed(D3DReadFileToBlob(L"TerrainVertexShader.cso", &vertexShaderBlob));
            BruteForce::DataBlob pixelShaderBlob;
            ThrowIfFailed(D3DReadFileToBlob(L"TerrainPixelShader.cso", &pixelShaderBlob));



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
            descRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 3, 0);
            descRange[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 2, 0);
            CD3DX12_ROOT_PARAMETER1 rootParameters[3];
            rootParameters[2].InitAsConstants(sizeof(BruteForce::Math::Matrix) / 4, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);
            //rootParameters[3].InitAsConstants(sizeof(BruteForce::Math::Matrix) / 4, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);

            rootParameters[0].InitAsDescriptorTable(1, &descRange[0], D3D12_SHADER_VISIBILITY_ALL);
            rootParameters[1].InitAsDescriptorTable(1, &descRange[1], D3D12_SHADER_VISIBILITY_ALL);

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
            m_PipelineState->SetName(L"Pipeline state");
            //Geometry::CreateCube(device, m_cube);
            Geometry::CreatePlane(device, m_plane, 100, 100, 6.0f, 6.0f);
        }


        SmartCommandList& RenderTerrain::PrepareRenderCommandList(SmartCommandList& smart_command_list, const RenderDestination& render_dest)
        {
            auto& commandList = smart_command_list.command_list;
            smart_command_list.SetPipelineState(m_PipelineState);
            smart_command_list.SetRootSignature(m_RootSignature);

            ID3D12DescriptorHeap* ppHeaps[] = { m_SVRHeap.Get(), m_SamplerHeap.Get() };
            commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

            commandList->SetGraphicsRootDescriptorTable(0,
                m_SVRHeap->GetGPUDescriptorHandleForHeapStart());
            commandList->SetGraphicsRootDescriptorTable(1,
                m_SamplerHeap->GetGPUDescriptorHandleForHeapStart());

            commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            commandList->IASetVertexBuffers(0, 1, &m_plane.m_VertexBufferView);
            commandList->IASetIndexBuffer(&m_plane.m_IndexBufferView);
            commandList->RSSetViewports(1, render_dest.m_Viewport);
            commandList->RSSetScissorRects(1, render_dest.m_ScissorRect);
            commandList->OMSetRenderTargets(1, render_dest.rtv, FALSE, render_dest.dsv);

            auto offset = sizeof(BruteForce::Math::Matrix) / 4;
            commandList->SetGraphicsRoot32BitConstants(2, offset, render_dest.camera.GetCameraMatrixPointer(), 0);
            commandList->DrawIndexedInstanced(m_plane.m_IndexesCount, 1, 0, 0, 0);
            return smart_command_list;
        }
    }
}