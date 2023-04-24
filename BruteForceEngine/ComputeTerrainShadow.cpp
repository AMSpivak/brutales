#include "ComputeTerrainShadow.h"
#include "Settings.h"
#include "GameEnvironment.h"

namespace BruteForce
{
    namespace Compute
    {
        void ComputeTerrainShadow::PrepareCB(uint32_t buff_index)
        {
			auto terrain_scaler = GlobalLevelInfo::ReadGlobalTerrainInfo().m_TerrainScaler;
			int shadow_size = GetTerrainShadowSize();
			//float lateral_scaler = 1 / terrain_scaler.x;
			const auto& sun_info = GlobalLevelInfo::ReadGlobalAtmosphereInfo();
			//Math::Vector sun_vec_tg = Math::Vector3Norm(Math::VectorSet(sun_info.m_SunInfo.x, 0.0, sun_info.m_SunInfo.z, 0.0));
			//Math::Vec4Float sun_info_dir;
			//Math::Store(&sun_info_dir, sun_vec_tg);


			//float lateral_scaler = 1 / 0.0002f;
			//float first_shadow_tg = sun_info.m_SunShadow.x * lateral_scaler / shadow_size;
			//float second_shadow_tg = sun_info.m_SunShadow.y * lateral_scaler / shadow_size;

            float shadow_tg_mul = 1.0f / (shadow_size * terrain_scaler.x);

			m_TerrainShadowBuffers[buff_index].m_CpuBuffer->srcTextureSize.x = shadow_size;
			m_TerrainShadowBuffers[buff_index].m_CpuBuffer->srcTextureSize.y = shadow_size;

			m_TerrainShadowBuffers[buff_index].m_CpuBuffer->LightSpace1 = { sun_info.m_SunShadow.z , sun_info.m_SunShadow.w ,
                                                                             abs(sun_info.m_SunShadow.z) + abs(sun_info.m_SunShadow.w),
																			0.0f };
			m_TerrainShadowBuffers[buff_index].m_CpuBuffer->LightSpace2 = { 0.0f, terrain_scaler.y,
                                                                            sun_info.m_SunShadow.x * shadow_tg_mul,
                                                                            sun_info.m_SunShadow.y * shadow_tg_mul };

			m_TerrainShadowBuffers[buff_index].m_CpuBuffer->LightSpace1_moon = { sun_info.m_MoonShadow.z , sun_info.m_MoonShadow.w ,
																sun_info.m_MoonShadowScaler,
																0.0f };
			m_TerrainShadowBuffers[buff_index].m_CpuBuffer->LightSpace2_moon = { 0.0f, terrain_scaler.y,
																sun_info.m_MoonShadow.x * shadow_tg_mul,
																sun_info.m_MoonShadow.y * shadow_tg_mul };
			m_TerrainShadowBuffers[buff_index].Update();

        }


        ComputeTerrainShadow::ComputeTerrainShadow() :m_TerrainShadowBuffers(nullptr)
        {
        }

        ComputeTerrainShadow::~ComputeTerrainShadow()
        {
            if (m_TerrainShadowBuffers)
            {
                delete[] m_TerrainShadowBuffers;
            }
        }
        void ComputeTerrainShadow::Update(float delta_time, uint8_t frame_index)
        {
        }

        void ComputeTerrainShadow::LoadContent(Device& device, uint8_t frames_count, DescriptorHeapManager& descriptor_heap_manager)
        {
            SunShadowUavDescriptors = descriptor_heap_manager.GetManagedRange("TerrainShadowUavs");
            assert(SunShadowUavDescriptors);

            HeightmapTexturesRange = descriptor_heap_manager.GetManagedRange("TerrainHeightmapTextures");
            assert(HeightmapTexturesRange);
            
            if (m_TerrainShadowBuffers)
            {
                delete[] m_TerrainShadowBuffers;
            }

            m_TerrainShadowBuffers = new ConstantBuffer<TerrainShadowCB>[frames_count];

            {
                CbvRange = descriptor_heap_manager.AllocateManagedRange(device, static_cast<UINT>(frames_count), BruteForce::DescriptorRangeTypeCvb, "TerrainCBVs");
                auto& cvb_handle = CbvRange->m_CpuHandle;//descriptor_heap_manager.AllocateRange(device, static_cast<UINT>(frames_count), CbvRange);

                for (int i = 0; i < frames_count; i++)
                {
                    CreateUploadGPUBuffer(device, m_TerrainShadowBuffers[i], cvb_handle);

                    m_TerrainShadowBuffers[i].Map();
                    m_TerrainShadowBuffers[i].Update();

                    cvb_handle.ptr += device->GetDescriptorHandleIncrementSize(BruteForce::DescriptorHeapCvbSrvUav);
                }
            }

            auto& settings = BruteForce::GetSettings();
            std::wstring content_path{ settings.GetExecuteDirWchar() };

            BruteForce::DataBlob ComputeShaderBlob;
            ThrowIfFailed(D3DReadFileToBlob((content_path + L"ComputeTerrainShadow.cso").c_str(), &ComputeShaderBlob));

            D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
            featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
            if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
            {
                featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
            }

            DescriptorRange descRange[3];

            //DescriptorRange srcMip;// (D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);
            //DescriptorRange outMip;// (D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);
            SunShadowUavDescriptors->Fill(descRange[1], 0);
            descRange[1].Flags = D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE;
            assert(HeightmapTexturesRange);
            HeightmapTexturesRange->Fill(descRange[0], 0);

            CbvRange->Fill(descRange[2], 1);




            CD3DX12_ROOT_PARAMETER1 rootParameters[2];
            rootParameters[0].InitAsConstants(1, 0);
            rootParameters[1].InitAsDescriptorTable(3, descRange);

            CD3DX12_STATIC_SAMPLER_DESC linearClampSampler(
                0,
                D3D12_FILTER_ANISOTROPIC,//D3D12_FILTER_MIN_MAG_MIP_LINEAR,
                D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
                D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
                D3D12_TEXTURE_ADDRESS_MODE_CLAMP
            );

            D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
                D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
                D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
                D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
                D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

            CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription;

            rootSignatureDescription.Init_1_1(_countof(rootParameters), rootParameters, 1, &linearClampSampler, rootSignatureFlags);

            BruteForce::DataBlob rootSignatureBlob;
            BruteForce::DataBlob errorBlob;
            ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDescription,
                featureData.HighestVersion, &rootSignatureBlob, &errorBlob));

            // Create the root signature.
            ThrowIfFailed(device->CreateRootSignature(0, rootSignatureBlob->GetBufferPointer(),
                rootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature)));
            m_RootSignature->SetName(L"Compute shadow RS");

            struct PipelineStateStream
            {
                CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRootSignature;
                CD3DX12_PIPELINE_STATE_STREAM_CS CS;
            } pipelineStateStream;

            pipelineStateStream.pRootSignature = m_RootSignature.Get();
            pipelineStateStream.CS = CD3DX12_SHADER_BYTECODE(ComputeShaderBlob.Get());

            D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = {
            sizeof(PipelineStateStream), &pipelineStateStream
            };

            ThrowIfFailed(device->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&m_PipelineState)));
            m_PipelineState->SetName(L"Compute shadow PSO");
        }
        SmartCommandList& ComputeTerrainShadow::PrepareRenderCommandList(SmartCommandList& smart_command_list, const PrepareComputeHelper& compute_helper)
        {
            smart_command_list.BeginEvent(0, "ComputeTerrainShadow");

            uint32_t buff_index = compute_helper.frame_index;
            PrepareCB(buff_index);


            auto& command_list = smart_command_list.command_list;
            smart_command_list.SetPipelineState(m_PipelineState);
            smart_command_list.SetComputeRootSignature(m_RootSignature);
            ID3D12DescriptorHeap* const ppHeaps[] = { compute_helper.HeapManager.GetDescriptorHeapPointer()};
            command_list->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

            command_list->SetComputeRootDescriptorTable(1, compute_helper.HeapManager.GetGpuDescriptorHandle());
            command_list->SetComputeRoot32BitConstants(0, 1, &buff_index, 0);

            int dispatch_size = 64;

            int shadow_size = GetTerrainShadowSize();

            command_list->Dispatch(static_cast<UINT>((shadow_size + dispatch_size - 1) / dispatch_size), 1, 1);
            //command_list
            // TODO: insert return statement here
            smart_command_list.EndEvent();
            return smart_command_list;
        }
    }
}
