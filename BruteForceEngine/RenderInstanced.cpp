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
                auto& object = m_objects.emplace_back();
                object.m_geometry = std::make_shared<IndexedGeometry>();

                Geometry::LoadGeometryGlb(device, *object.m_geometry, content_dir_path + L"wall.glb");

                BruteForce::Textures::TextureLoadHlpr helper{ device, copy_queue, desc.gpu_allocator_ptr };


                const std::wstring textures[] = { {L"wall.dds"}, {L"wall_n.dds"}, {L""} };


                for (int i = 0; i < 1; i++)
                {
                    object.m_material = desc.m_MaterialManager->AddMaterial(textures[i * 3], textures[i * 3 + 1], textures[i * 3 + 2]);
                }
            }
            BruteForce::DataBlob StaticVertexShaderBlob;
            ThrowIfFailed(D3DReadFileToBlob((content_path + L"BasicVertexShader.cso").c_str(), &StaticVertexShaderBlob));

            BruteForce::DataBlob pixelShaderBlob;
            ThrowIfFailed(D3DReadFileToBlob((content_path + L"BasicPixelShader.cso").c_str(), &pixelShaderBlob));



            D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
                { "POSITION", 0, TargetFormat_R32G32B32_Float, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
                { "UV", 0, TargetFormat_R32G32_Float, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
                { "NORMAL", 0, TargetFormat_R32G32B32_Float, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
                { "TANGENT", 0, TargetFormat_R32G32B32A32_Float, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
            };


            D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
                D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
                D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
                D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
                D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;// |
                //D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;
            CD3DX12_ROOT_PARAMETER1 rootParameters[4];
            rootParameters[0].InitAsConstants(sizeof(BruteForce::Math::Matrix) / 4, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);
            rootParameters[1].InitAsConstants(sizeof(BruteForce::Math::Matrix) / 4, 5, 0, D3D12_SHADER_VISIBILITY_VERTEX);
			rootParameters[2].InitAsConstants(sizeof(uint32_t) / 4, 10, 0, D3D12_SHADER_VISIBILITY_VERTEX);
			rootParameters[3].InitAsConstants(sizeof(BruteForce::Math::Vec4Float) / 4, 11, 0, D3D12_SHADER_VISIBILITY_VERTEX);
            
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
            pipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(StaticVertexShaderBlob.Get());
            pipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(pixelShaderBlob.Get());
            pipelineStateStream.DSVFormat = TargetFormat_D32_Float;
            pipelineStateStream.RTVFormats = rtvFormats;

            D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = {
                sizeof(PipelineStateStream), &pipelineStateStream
            };
            ThrowIfFailed(device->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&m_PipelineState)));

        }


        SmartCommandList& RenderInstanced::PrepareRenderCommandList(SmartCommandList& smart_command_list, const PrepareRenderHelper& render_dest)
        {
            auto& commandList = smart_command_list.command_list;
            smart_command_list.SetPipelineState(m_PipelineState);
            smart_command_list.SetGraphicsRootSignature(m_RootSignature);


            commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            commandList->RSSetViewports(1, render_dest.m_Viewport);
            commandList->RSSetScissorRects(1, render_dest.m_ScissorRect);
            commandList->OMSetRenderTargets(render_dest.m_rt_count, render_dest.rtv, FALSE, render_dest.dsv);

            auto const_size = sizeof(BruteForce::Math::Matrix) / 4;

            for(int obj_i = 0; obj_i < m_objects.size(); obj_i++)
            {
                
                auto& obj = m_objects[obj_i];
                commandList->IASetVertexBuffers(0, 1, &obj.m_geometry->m_VertexBufferView);
                commandList->IASetIndexBuffer(&obj.m_geometry->m_IndexBufferView);
                commandList->SetGraphicsRoot32BitConstants(0, static_cast<UINT>(const_size), render_dest.camera.GetCameraMatrixPointer(), 0);

                BruteForce::Math::Matrix M(1.f, 0.f, 0.f, 0.f,
                    0.f, 1.f, 0.f, 0.f,
                    0.f, 0.f, 1.f, 0.f,
                    0.f, 20.f, 30.f + 5.0f* obj_i, 1.f);
                commandList->SetGraphicsRoot32BitConstants(1, static_cast<UINT>(const_size), &M, 0);

                uint32_t material_id = obj.m_material->m_index;
                commandList->SetGraphicsRoot32BitConstants(2, static_cast<UINT>(sizeof(uint32_t) / 4), &material_id, 0);
				BruteForce::Math::Vec4Float clipplane{ -1.0f, 0.0f, 0.0f, 0.0f };
				//BruteForce::Math::Vec4Float clipplane{ 0.0f, 0.0f, 0.0f, 0.0f };
				commandList->SetGraphicsRoot32BitConstants(3, static_cast<UINT>(sizeof(BruteForce::Math::Vec4Float) / 4), &clipplane, 0);

                commandList->DrawIndexedInstanced(static_cast<UINT>(obj.m_geometry->m_IndexesCount), 1, 0, 0, 0);
            }
            return smart_command_list;
        }
    }
}