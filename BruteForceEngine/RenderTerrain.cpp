#include "RenderTerrain.h"
#include "Helpers.h"
#include "IndexedGeometryGenerator.h"
#include "Settings.h"
#include "GameEnvironment.h"


namespace BruteForce
{
    namespace Render
    {
        UINT RenderTerrain::PreparePlanesCB(const Math::Vec4Float& cam, uint32_t index)
        {
            m_TerrainBuffers[index].m_CpuBuffer->m_PlanesPositions[0] = Math::Vec4Float{ cam.x + 1.0f, cam.z + 1.0f,1.0f, 1.0f };
            m_TerrainBuffers[index].m_CpuBuffer->m_PlanesPositions[1] = Math::Vec4Float{ cam.x + -1.0f, cam.z + 1.0f,1.0f, 1.0f };
            m_TerrainBuffers[index].m_CpuBuffer->m_PlanesPositions[2] = Math::Vec4Float{ cam.x + 1.0f, cam.z + -1.0f,1.0f, 1.0f };
            m_TerrainBuffers[index].m_CpuBuffer->m_PlanesPositions[3] = Math::Vec4Float{ cam.x + -1.0f, cam.z + -1.0f,1.0f, 1.0f };

            UINT counter = 4;
            float s_offset = 1.0f;
            float h_offset = 3.0f;
            float plate_half_widht = 1.0f;


            constexpr UINT buffer_size = 1024 - 12;

            while (counter < buffer_size)
            {

                m_TerrainBuffers[index].m_CpuBuffer->m_PlanesPositions[counter++] = Math::Vec4Float{ cam.x + s_offset, cam.z + h_offset, plate_half_widht, 1.0f };
                m_TerrainBuffers[index].m_CpuBuffer->m_PlanesPositions[counter++] = Math::Vec4Float{ cam.x + h_offset, cam.z + s_offset, plate_half_widht, 1.0f };
                m_TerrainBuffers[index].m_CpuBuffer->m_PlanesPositions[counter++] = Math::Vec4Float{ cam.x + h_offset, cam.z + h_offset, plate_half_widht, 1.0f };

                m_TerrainBuffers[index].m_CpuBuffer->m_PlanesPositions[counter++] = Math::Vec4Float{ cam.x - s_offset, cam.z - h_offset, plate_half_widht, 1.0f };
                m_TerrainBuffers[index].m_CpuBuffer->m_PlanesPositions[counter++] = Math::Vec4Float{ cam.x - h_offset, cam.z - s_offset, plate_half_widht, 1.0f };
                m_TerrainBuffers[index].m_CpuBuffer->m_PlanesPositions[counter++] = Math::Vec4Float{ cam.x - h_offset, cam.z - h_offset, plate_half_widht, 1.0f };

                m_TerrainBuffers[index].m_CpuBuffer->m_PlanesPositions[counter++] = Math::Vec4Float{ cam.x + s_offset, cam.z - h_offset, plate_half_widht, 1.0f };
                m_TerrainBuffers[index].m_CpuBuffer->m_PlanesPositions[counter++] = Math::Vec4Float{ cam.x + h_offset, cam.z - s_offset, plate_half_widht, 1.0f };
                m_TerrainBuffers[index].m_CpuBuffer->m_PlanesPositions[counter++] = Math::Vec4Float{ cam.x + h_offset, cam.z - h_offset, plate_half_widht, 1.0f };

                m_TerrainBuffers[index].m_CpuBuffer->m_PlanesPositions[counter++] = Math::Vec4Float{ cam.x - s_offset, cam.z + h_offset, plate_half_widht, 1.0f };
                m_TerrainBuffers[index].m_CpuBuffer->m_PlanesPositions[counter++] = Math::Vec4Float{ cam.x - h_offset, cam.z + s_offset, plate_half_widht, 1.0f };
                m_TerrainBuffers[index].m_CpuBuffer->m_PlanesPositions[counter++] = Math::Vec4Float{ cam.x - h_offset, cam.z + h_offset, plate_half_widht, 1.0f };

                s_offset *= 2.0f;
                h_offset *= 2.0f;
                plate_half_widht *= 2.0f;
            }
            float plane_mesh_step = 1.0f / 100;
            
            auto terrain_scaler = GlobalLevelInfo::ReadGlobalTerrainInfo().m_TerrainScaler;
            terrain_scaler.w = plane_mesh_step;
            m_TerrainBuffers[index].m_CpuBuffer->m_TerrainScaler = terrain_scaler;

            const auto& sun_info = GlobalLevelInfo::ReadGlobalAtmosphereInfo();
            m_TerrainBuffers[index].m_CpuBuffer->m_SunInfo = sun_info.m_SunInfo;

            m_TerrainBuffers[index].m_CpuBuffer->m_SunShadow = { sun_info.m_SunShadow.z, -sun_info.m_SunShadow.w,
                 1.0f / sun_info.m_SunShadowScaler,
                0.0f};

            m_TerrainBuffers[index].Update();
            
            return counter;
        }

        RenderTerrain::RenderTerrain() :m_TerrainBuffers(nullptr)
        {

        }

        RenderTerrain::~RenderTerrain()
        {
            if (m_TerrainBuffers)
            {
                delete[] m_TerrainBuffers;
            }
        }
        void RenderTerrain::Update(float delta_time, uint8_t frame_index)
        {
        }

        void RenderTerrain::LoadContent(Device& device, uint8_t frames_count, const RenderSubsystemInitDesc& desc, SmartCommandQueue& copy_queue, DescriptorHeapManager& descriptor_heap_manager)
        {
            if (m_TerrainBuffers)
            {
                delete[] m_TerrainBuffers;
            }

            m_TerrainBuffers = new ConstantBuffer<TerrainVertexCB>[frames_count];

            auto& settings = BruteForce::GetSettings();
            std::wstring content_path{ settings.GetExecuteDirWchar() };

            {
                D3D12_DESCRIPTOR_HEAP_DESC descHeapSampler = {};
                descHeapSampler.NumDescriptors = 1;
                descHeapSampler.Type = BruteForce::DescriptorHeapSampler;
                descHeapSampler.Flags = BruteForce::DescriptorHeapShaderVisible;
                ThrowIfFailed(device->CreateDescriptorHeap(&descHeapSampler, __uuidof(ID3D12DescriptorHeap), (void**)&m_SamplerHeap));
                m_SamplerHeap->SetName(L"Sampler heap");

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
                auto sampler_handle = m_SamplerHeap->GetCPUDescriptorHandleForHeapStart();
                device->CreateSampler(&samplerDesc, sampler_handle);
            }

            //DescriptorHeapRange TexturesRange{ DescriptorRangeTypeSrv,"TerrainTextures"};
            //DescriptorHeapRange CbvRange{ DescriptorRangeTypeCvb,"TerrainCBVs"};

            {
                CbvRange = descriptor_heap_manager.AllocateManagedRange(device, static_cast<UINT>(frames_count), BruteForce::DescriptorRangeTypeCvb, "TerrainCBVs");
                auto& srv_handle = CbvRange->m_CpuHandle;//descriptor_heap_manager.AllocateRange(device, static_cast<UINT>(frames_count), CbvRange);

                for (int i = 0; i < frames_count; i++)
                {
                    CreateUploadGPUBuffer(device, m_TerrainBuffers[i], srv_handle);

                    m_TerrainBuffers[i].Map();
                    m_TerrainBuffers[i].Update();

                    srv_handle.ptr += device->GetDescriptorHandleIncrementSize(BruteForce::DescriptorHeapCvbSrvUav);
                }
            }

            {
                {
                    HeightmapTexturesRange = descriptor_heap_manager.GetManagedRange("TerrainHeightmapTextures");
                    assert(HeightmapTexturesRange);
                    auto& srv_handle = HeightmapTexturesRange->m_CpuHandle;//descriptor_heap_manager.AllocateRange(device, static_cast<UINT>(textures_count), TexturesRange);
                    BruteForce::Textures::AddTexture(content_path, { L"desert_map_16_2.png" }, m_textures, device, copy_queue, srv_handle);
                    BruteForce::Textures::AddTexture(content_path, { L"map_materials.png" }, m_textures, device, copy_queue, srv_handle, TargetFormat_R8G8B8A8_UInt);
                }

                std::vector<std::wstring> tex_names = { { L"Desert_Rock_albedo.dds"}, {L"Desert_Sand_albedo.dds"} };
                size_t textures_count = tex_names.size();
                TexturesRange = descriptor_heap_manager.AllocateManagedRange(device, static_cast<UINT>(textures_count), BruteForce::DescriptorRangeTypeSrv, "TerrainMaterialTextures");
                auto& srv_handle = TexturesRange->m_CpuHandle;
                BruteForce::Textures::AddTextures(tex_names.begin(), tex_names.end(), content_path, m_textures, device, copy_queue, srv_handle);
            }

            SunShadowSrvDescriptors = descriptor_heap_manager.GetManagedRange("TerrainShadowSrvs");

            BruteForce::DataBlob vertexShaderBlob;
            ThrowIfFailed(D3DReadFileToBlob((content_path + L"TerrainVertexShader.cso").c_str(), &vertexShaderBlob));
            BruteForce::DataBlob pixelShaderBlob;
            ThrowIfFailed(D3DReadFileToBlob((content_path + L"TerrainPixelShader.cso").c_str(), &pixelShaderBlob));



            D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
                { "POSITION", 0, TargetFormat_R32G32B32_Float, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
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
                D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

            DescriptorRange descRange[4];
            TexturesRange->Fill(descRange[0], 5);
            HeightmapTexturesRange->Fill(descRange[1], 0);
            CbvRange->Fill(descRange[2], 17);
            SunShadowSrvDescriptors->Fill(descRange[3], 2);

            CD3DX12_DESCRIPTOR_RANGE1 descRangeSamp;
            descRangeSamp.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0);

            CD3DX12_ROOT_PARAMETER1 rootParameters[4];
            rootParameters[3].InitAsConstants(sizeof(BruteForce::Math::Matrix) / 4, 1, 0, D3D12_SHADER_VISIBILITY_VERTEX);
            rootParameters[2].InitAsConstants(1, 0, 0, D3D12_SHADER_VISIBILITY_ALL);
            rootParameters[0].InitAsDescriptorTable(_countof(descRange), descRange, D3D12_SHADER_VISIBILITY_ALL);
            rootParameters[1].InitAsDescriptorTable(1, &descRangeSamp, D3D12_SHADER_VISIBILITY_ALL);

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
            m_RootSignature->SetName(L"Render terrain RS");

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
            rtvFormats.NumRenderTargets = 1;
            rtvFormats.RTFormats[0] = desc.RTFormat;

            pipelineStateStream.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

            pipelineStateStream.pRootSignature = m_RootSignature.Get();
            pipelineStateStream.InputLayout = { inputLayout, _countof(inputLayout) };
            pipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
            pipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(vertexShaderBlob.Get());
            pipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(pixelShaderBlob.Get());
            pipelineStateStream.DSVFormat = desc.DepthFormat;
            pipelineStateStream.RTVFormats = rtvFormats;

            D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = {
                sizeof(PipelineStateStream), &pipelineStateStream
            };
            ThrowIfFailed(device->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&m_PipelineState)));
            m_PipelineState->SetName(L"Render terrain PSO");
            Geometry::CreatePlane<VertexPos>(device, m_plane, 100, 100, 1.0f, 1.0f);
            //Geometry::CreatePlane<VertexPos>(device, m_plane, 3, 3, 1.0f, 1.0f);
        }


        SmartCommandList& RenderTerrain::PrepareRenderCommandList(SmartCommandList& smart_command_list, const PrepareRenderHelper& render_dest)
        {
            //m_TerrainBuffers[0].m_CpuBuffer->m_TerrainScaler = Math::Vec3Float{ 1.0f,1.0f, };
            Math::Vec4Float cam;
            Math::Store(&cam, render_dest.camera.GetPosition());
            uint32_t buff_index = render_dest.frame_index;

            UINT counter = PreparePlanesCB(cam, buff_index);
            auto& commandList = smart_command_list.command_list;
            smart_command_list.SetPipelineState(m_PipelineState);
            smart_command_list.SetGraphicsRootSignature(m_RootSignature);

            ID3D12DescriptorHeap* const ppHeaps[] = { render_dest.HeapManager.GetDescriptorHeapPointer(), m_SamplerHeap.Get()};
            commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

            commandList->SetGraphicsRootDescriptorTable(0, render_dest.HeapManager.GetGpuDescriptorHandle());
            commandList->SetGraphicsRootDescriptorTable(1, m_SamplerHeap->GetGPUDescriptorHandleForHeapStart());

            commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            commandList->IASetVertexBuffers(0, 1, &m_plane.m_VertexBufferView);
            commandList->IASetIndexBuffer(&m_plane.m_IndexBufferView);
            commandList->RSSetViewports(1, render_dest.m_Viewport);
            commandList->RSSetScissorRects(1, render_dest.m_ScissorRect);
            commandList->OMSetRenderTargets(1, render_dest.rtv, FALSE, render_dest.dsv);

            auto const_size = sizeof(BruteForce::Math::Matrix) / 4;
            commandList->SetGraphicsRoot32BitConstants(3, static_cast<UINT>(const_size), render_dest.camera.GetCameraMatrixPointer(), 0);
            commandList->SetGraphicsRoot32BitConstants(2, 1, &buff_index, 0);
            commandList->DrawIndexedInstanced(static_cast<UINT>(m_plane.m_IndexesCount), counter, 0, 0, 0);
            return smart_command_list;
        }
    }
}