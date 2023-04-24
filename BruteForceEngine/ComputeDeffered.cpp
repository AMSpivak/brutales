#include "ComputeDeffered.h"
#include "Settings.h"
#include "GameEnvironment.h"

namespace BruteForce
{
	namespace Compute
	{
		ComputeDeffered::ComputeDeffered() :m_DefferedBuffers(nullptr)
		{
		}

		ComputeDeffered::~ComputeDeffered()
		{
			if (m_DefferedBuffers)
			{
				delete[] m_DefferedBuffers;
			}
		}
		void ComputeDeffered::Update(float delta_time, uint8_t frame_index)
		{
		}
		void ComputeDeffered::LoadContent(Device& device, uint8_t frames_count, DescriptorHeapManager& descriptor_heap_manager)
		{
			//LuminanceUavDescriptors = descriptor_heap_manager.GetManagedRange("LuminanceUavs");
			//assert(LuminanceUavDescriptors);
			SrvTexturesRange = descriptor_heap_manager.GetManagedRange("TerrainMaterialTextures");
			assert(SrvTexturesRange);

			RTUavDescriptors = descriptor_heap_manager.GetManagedRange("RenderTargetsUavs");
			assert(RTUavDescriptors);

			RTNoScreenSrvDescriptors = descriptor_heap_manager.GetManagedRange("NoScreenRenderTargetsSrvs");
			assert(RTNoScreenSrvDescriptors);

			SunShadowSrvDescriptors = descriptor_heap_manager.GetManagedRange("TerrainShadowSrvs");
			assert(SunShadowSrvDescriptors);

			DepthSrvDescriptors = descriptor_heap_manager.GetManagedRange("DepthSrvs");
			assert(DepthSrvDescriptors);

			if (m_DefferedBuffers)
			{
				delete[] m_DefferedBuffers;
			}
			//static constexpr size_t cb_count = frames_count;
			m_DefferedBuffers = new ConstantBuffer<DefferedLightingCB>[frames_count];

			{
				CbvRange = descriptor_heap_manager.AllocateManagedRange(device, static_cast<UINT>(frames_count), BruteForce::DescriptorRangeTypeCvb, "LuminanceCBVs");
				auto& cvb_handle = CbvRange->m_CpuHandle;//descriptor_heap_manager.AllocateRange(device, static_cast<UINT>(frames_count), CbvRange);

				for (int i = 0; i < frames_count; i++)
				{
					CreateUploadGPUBuffer(device, m_DefferedBuffers[i], cvb_handle);

					m_DefferedBuffers[i].Map();
					m_DefferedBuffers[i].Update();

					cvb_handle.ptr += device->GetDescriptorHandleIncrementSize(BruteForce::DescriptorHeapCvbSrvUav);
				}
			}

			auto& settings = BruteForce::GetSettings();
			std::wstring content_path{ settings.GetExecuteDirWchar() };

			D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
				D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;

			BruteForce::DataBlob ComputeShaderBlob;

			struct PipelineStateStream
			{
				CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRootSignature;
				CD3DX12_PIPELINE_STATE_STREAM_CS CS;
			} pipelineStateStream;

			D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};
			featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
			if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
			{
				featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
			}

			{

				ThrowIfFailed(D3DReadFileToBlob((content_path + L"ComputeDeffered.cso").c_str(), &ComputeShaderBlob));



				DescriptorRange descRange[6];

				//DescriptorRange srcMip;// (D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);
				//DescriptorRange outMip;// (D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);


				RTUavDescriptors->Fill(descRange[0], 0);
				//descRange[1].Flags = D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE;
				DepthSrvDescriptors->Fill(descRange[5], 8);
				SrvTexturesRange->Fill(descRange[1], 9);

				CbvRange->Fill(descRange[2], 2);

				RTNoScreenSrvDescriptors->Fill(descRange[4], 3);
				SunShadowSrvDescriptors->Fill(descRange[3], 0);

				CD3DX12_DESCRIPTOR_RANGE1 descRangeSamp;
				descRangeSamp.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0);

				CD3DX12_ROOT_PARAMETER1 rootParameters[2];
				rootParameters[1].InitAsConstants(1, 0, 0);
				rootParameters[0].InitAsDescriptorTable(_countof(descRange), descRange);

				

				CD3DX12_STATIC_SAMPLER_DESC AnisotropicClampSampler(
					0,
					//D3D12_FILTER_MIN_MAG_MIP_LINEAR,
					D3D12_FILTER_ANISOTROPIC,
					D3D12_TEXTURE_ADDRESS_MODE_WRAP,
					D3D12_TEXTURE_ADDRESS_MODE_WRAP,
					D3D12_TEXTURE_ADDRESS_MODE_WRAP
				);



				CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription;

				//rootSignatureDescription.Init_1_1(_countof(rootParameters), rootParameters, 1, &linearClampSampler, rootSignatureFlags);
				rootSignatureDescription.Init_1_1(_countof(rootParameters), rootParameters, 1, &AnisotropicClampSampler, rootSignatureFlags);

				BruteForce::DataBlob rootSignatureBlob;
				BruteForce::DataBlob errorBlob;
				ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDescription,
					featureData.HighestVersion, &rootSignatureBlob, &errorBlob));

				// Create the root signature.
				ThrowIfFailed(device->CreateRootSignature(0, rootSignatureBlob->GetBufferPointer(),
					rootSignatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_RootSignature)));
				m_RootSignature->SetName(L"Compute Deffered RS");

				pipelineStateStream.pRootSignature = m_RootSignature.Get();
				pipelineStateStream.CS = CD3DX12_SHADER_BYTECODE(ComputeShaderBlob.Get());

				D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = {
				sizeof(PipelineStateStream), &pipelineStateStream
				};

				ThrowIfFailed(device->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&m_PipelineState)));
				m_PipelineState->SetName(L"Compute Deffered PSO");
			}
		}

		SmartCommandList& ComputeDeffered::PrepareRenderCommandList(SmartCommandList& smart_command_list, const PrepareComputeHelper& compute_helper)
		{
			uint32_t buff_index = compute_helper.frame_index;

			const auto& sun_info = GlobalLevelInfo::ReadGlobalAtmosphereInfo();
			m_DefferedBuffers[buff_index].m_CpuBuffer->m_SunInfo = sun_info.m_SunInfo;
			m_DefferedBuffers[buff_index].m_CpuBuffer->m_MoonInfo = sun_info.m_MoonInfo;

			m_DefferedBuffers[buff_index].m_CpuBuffer->m_SunShadow = { sun_info.m_SunShadow.z, -sun_info.m_SunShadow.w,
				 //1.0f / sun_info.m_SunShadowScaler,
				 1.0f / (abs(sun_info.m_SunShadow.z) + abs(sun_info.m_SunShadow.w)),
				0.0f };
			m_DefferedBuffers[buff_index].m_CpuBuffer->m_SunColor = sun_info.m_SunColor;
			m_DefferedBuffers[buff_index].m_CpuBuffer->m_MoonColor = sun_info.m_MoonColor;
			m_DefferedBuffers[buff_index].m_CpuBuffer->m_CameraInverse = *(compute_helper.camera.GetInverseCameraMatrixPointer());
			Math::Store(&(m_DefferedBuffers[buff_index].m_CpuBuffer->m_CameraPosition), compute_helper.camera.GetPosition());
			float plane_mesh_step = 1.0f / 100;

			auto terrain_scaler = GlobalLevelInfo::ReadGlobalTerrainInfo().m_TerrainScaler;
			terrain_scaler.w = plane_mesh_step;
			m_DefferedBuffers[buff_index].m_CpuBuffer->m_TerrainScaler = terrain_scaler;

			m_DefferedBuffers[buff_index].Update();
			
			smart_command_list.BeginEvent(0, "ComputeDeffered");

			auto& command_list = smart_command_list.command_list;
			smart_command_list.SetPipelineState(m_PipelineState);
			smart_command_list.SetComputeRootSignature(m_RootSignature);
			ID3D12DescriptorHeap* const ppHeaps[] = { compute_helper.HeapManager.GetDescriptorHeapPointer() };
			command_list->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

			command_list->SetComputeRootDescriptorTable(0, compute_helper.HeapManager.GetGpuDescriptorHandle());
			command_list->SetComputeRoot32BitConstants(1, 1, &buff_index, 0);

			
			int dispatch_size = DEFFERED_DISPATCH_SIZE;
			UINT dispatch_x = static_cast<UINT>((compute_helper.m_Viewport->Width + dispatch_size - 1) / dispatch_size);
			UINT dispatch_y = static_cast<UINT>((compute_helper.m_Viewport->Height + dispatch_size - 1) / dispatch_size);
			command_list->Dispatch(dispatch_x, dispatch_y, 1);
			
			smart_command_list.EndEvent();
			return smart_command_list;
		}
	}
}
