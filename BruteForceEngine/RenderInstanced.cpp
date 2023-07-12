#include "RenderInstanced.h"
#include "Helpers.h"
#include "IndexedGeometryGenerator.h"
#include "Settings.h"
#include "GameEnvironment.h"
#include "CommonRenderParams.h"
namespace BruteForce
{
    namespace Render
    {
        RenderInstanced::RenderInstanced()
        {
        }
        RenderInstanced::~RenderInstanced()
        {
            
        }
        void RenderInstanced::Update(float delta_time, uint8_t frame_index)
        {
        }
        void RenderInstanced::LoadContent(Device& device, uint8_t frames_count, const RenderSubsystemInitDesc& desc, SmartCommandQueue& copy_queue, DescriptorHeapManager& descriptor_heap_manager)
        {
            auto& settings = BruteForce::GetSettings();
            std::wstring content_path{ settings.GetExecuteDirWchar() };

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
                samplerDesc.MaxLOD = BruteForce::Math::floatMax;
                samplerDesc.MipLODBias = 0.0f;
                samplerDesc.MaxAnisotropy = 1;
                samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
                device->CreateSampler(&samplerDesc, m_SamplerHeap->GetCPUDescriptorHandleForHeapStart());


            }
            {
                std::wstring content_dir_path{ settings.GetContentDirWchar() };
                std::vector<std::wstring> tex_names = { { L"test1.png"} ,{L"test2.png"} };
                size_t textures_count = tex_names.size();

                BruteForce::DescriptorHeapDesc descHeapCbvSrv = {};
                descHeapCbvSrv.NumDescriptors = static_cast<UINT>(textures_count);
                descHeapCbvSrv.Type = BruteForce::DescriptorHeapCvbSrvUav;
                descHeapCbvSrv.Flags = BruteForce::DescriptorHeapShaderVisible;
                ThrowIfFailed(device->CreateDescriptorHeap(&descHeapCbvSrv, __uuidof(ID3D12DescriptorHeap), (void**)&m_SVRHeap));


                auto srv_handle = m_SVRHeap->GetCPUDescriptorHandleForHeapStart();
                BruteForce::Textures::AddTextures(tex_names.begin(), tex_names.end(), content_dir_path, m_textures, device, srv_handle);
            }

            {
                std::wstring content_dir_path{ settings.GetContentDirWchar() };
                //Geometry::LoadGeometryGlb(device, m_cube, content_dir_path + L"barbarian_game.glb");
                Geometry::CreateCube(device, m_cube);
            }

            BruteForce::DataBlob vertexShaderBlob;
            ThrowIfFailed(D3DReadFileToBlob((content_path + L"BasicVertexShader.cso").c_str(), &vertexShaderBlob));

            BruteForce::DataBlob pixelShaderBlob;
            ThrowIfFailed(D3DReadFileToBlob((content_path + L"BasicPixelShader.cso").c_str(), &pixelShaderBlob));



            D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
                { "POSITION", 0, TargetFormat_R32G32B32_Float, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
                { "UV", 0, TargetFormat_R32G32_Float, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
                { "NORMAL", 0, TargetFormat_R32G32B32_Float, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
                { "TANGENT", 0, TargetFormat_R32G32B32_Float, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            };



            D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
                D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
                D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
                D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
                D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;// |
                //D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;
            CD3DX12_ROOT_PARAMETER1 rootParameters[1];
            rootParameters[0].InitAsConstants(sizeof(BruteForce::Math::Matrix) / 4, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);
            //rootParameters[3].InitAsConstants(sizeof(BruteForce::Math::Matrix) / 4, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);

            //CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDescription;
            //rootSignatureDescription.Init(3, rootParameters);
            CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription;

            rootSignatureDescription.Init_1_1(_countof(rootParameters), rootParameters, 0, nullptr, rootSignatureFlags);
            
            {
                D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
                featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
                if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
                {
                    featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
                }

                BruteForce::DataBlob rootSignatureBlob;
                BruteForce::DataBlob errorBlob;
                ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDescription,
                    featureData.HighestVersion, &rootSignatureBlob, &errorBlob));

                // Create the root signature.
                ThrowIfFailed(device->CreateRootSignature(0, rootSignatureBlob->GetBufferPointer(),
                    rootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature)));
            }


            struct PipelineStateStream
            {
                CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRootSignature;
                CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
                CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
                CD3DX12_PIPELINE_STATE_STREAM_VS VS;
                CD3DX12_PIPELINE_STATE_STREAM_PS PS;
                CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL DepthStencilState;
                CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
                CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
            } pipelineStateStream;

            D3D12_RT_FORMAT_ARRAY rtvFormats = {};
            rtvFormats.NumRenderTargets = NoScreenTextures;
            rtvFormats.RTFormats[0] = render_normals_format;
            rtvFormats.RTFormats[1] = render_materials_format;
            rtvFormats.RTFormats[2] = render_uv_format;
            rtvFormats.RTFormats[3] = render_uvddxddy_format;

            pipelineStateStream.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);;

            pipelineStateStream.pRootSignature = m_RootSignature.Get();
            pipelineStateStream.InputLayout = { inputLayout, _countof(inputLayout) };
            pipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
            pipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(vertexShaderBlob.Get());
            pipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(pixelShaderBlob.Get());
            pipelineStateStream.DSVFormat = TargetFormat_D32_Float;
            pipelineStateStream.RTVFormats = rtvFormats;

            D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = {
                sizeof(PipelineStateStream), &pipelineStateStream
            };
            ThrowIfFailed(device->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&m_PipelineState)));

            //Geometry::CreateCube(device, m_cube);
        }


        SmartCommandList& RenderInstanced::PrepareRenderCommandList(SmartCommandList& smart_command_list, const PrepareRenderHelper& render_dest)
        {
            auto& commandList = smart_command_list.command_list;
            smart_command_list.SetPipelineState(m_PipelineState);
            smart_command_list.SetGraphicsRootSignature(m_RootSignature);

            /*ID3D12DescriptorHeap* ppHeaps[] = { m_SVRHeap.Get(), m_SamplerHeap.Get() };
            commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

            commandList->SetGraphicsRootDescriptorTable(0,
                m_SVRHeap->GetGPUDescriptorHandleForHeapStart());
            commandList->SetGraphicsRootDescriptorTable(1,
                m_SamplerHeap->GetGPUDescriptorHandleForHeapStart());*/

            commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            commandList->IASetVertexBuffers(0, 1, &m_cube.m_VertexBufferView);
            commandList->IASetIndexBuffer(&m_cube.m_IndexBufferView);
            commandList->RSSetViewports(1, render_dest.m_Viewport);
            commandList->RSSetScissorRects(1, render_dest.m_ScissorRect);
            //commandList->OMSetRenderTargets(1, render_dest.rtv, FALSE, render_dest.dsv);
            commandList->OMSetRenderTargets(render_dest.m_rt_count, render_dest.rtv, FALSE, render_dest.dsv);
            auto const_size = sizeof(BruteForce::Math::Matrix) / 4;
            commandList->SetGraphicsRoot32BitConstants(0, static_cast<UINT>(const_size), render_dest.camera.GetCameraMatrixPointer(), 0);


            commandList->DrawIndexedInstanced(static_cast<UINT>(m_cube.m_IndexesCount), 1, 0, 0, 0);
            return smart_command_list;
        }
    }
}