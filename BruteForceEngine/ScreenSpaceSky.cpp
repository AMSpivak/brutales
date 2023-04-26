#include "ScreenSpaceSky.h"
#include "Settings.h"
#include "Helpers.h"
#include "Resources.h"
#include "GameEnvironment.h"

namespace BruteForce
{
    namespace Render
    {
        ScreenSpaceSky::~ScreenSpaceSky()
        {
            if (m_SkyBuffers)
            {
                delete[] m_SkyBuffers;
            }
        }
        void ScreenSpaceSky::Update(float delta_time, uint8_t frame_index)
        {
        }

        void ScreenSpaceSky::LoadContent(Device& device, uint8_t frames_count, const RenderSubsystemInitDesc& desc, SmartCommandQueue& copy_queue, DescriptorHeapManager& descriptor_heap_manager)
        {

            if (m_SkyBuffers)
            {
                delete[] m_SkyBuffers;
            }

            m_SkyBuffers = new ConstantBuffer<SkyPixelCB>[frames_count];

            {
                CbvRange = descriptor_heap_manager.AllocateManagedRange(device, static_cast<UINT>(frames_count), BruteForce::DescriptorRangeTypeCvb, "SkyCBVs");
                auto& srv_handle = CbvRange->m_CpuHandle;//descriptor_heap_manager.AllocateRange(device, static_cast<UINT>(frames_count), CbvRange);

                for (int i = 0; i < frames_count; i++)
                {
                    CreateUploadGPUBuffer(device, m_SkyBuffers[i], srv_handle);

                    m_SkyBuffers[i].Map();
                    m_SkyBuffers[i].Update();

                    srv_handle.ptr += device->GetDescriptorHandleIncrementSize(BruteForce::DescriptorHeapCvbSrvUav);
                }
            }

            auto& settings = BruteForce::GetSettings();
            std::wstring content_path{ settings.GetExecuteDirWchar() };

			{
				BruteForce::Textures::TextureLoadHlpr helper{ device, copy_queue, desc.gpu_allocator_ptr };

				std::vector<std::wstring> tex_names = {
														{L"moon_diff.png"}//{L"Desert_Sand_normal.dds"}//{L"norm_tst.png"}//
														,{ L"moon_normal.png"}//{ L"Desert_Rock_normal.dds"}
				};
				size_t textures_count = tex_names.size();
				TexturesRange = descriptor_heap_manager.AllocateManagedRange(device, static_cast<UINT>(textures_count), BruteForce::DescriptorRangeTypeSrv, "SkyTextures");
				auto& srv_handle = TexturesRange->m_CpuHandle;
				BruteForce::Textures::AddTextures(tex_names.begin(), tex_names.end(), content_path, m_textures, helper, srv_handle);
			}



            BruteForce::DataBlob vertexShaderBlob;
            ThrowIfFailed(D3DReadFileToBlob((content_path + L"SkyVertexShader.cso").c_str(), &vertexShaderBlob));
            BruteForce::DataBlob pixelShaderBlob;
            ThrowIfFailed(D3DReadFileToBlob((content_path + L"SkyPixelShader.cso").c_str(), &pixelShaderBlob));

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

            DescriptorRange descRange[2];
			CbvRange->Fill(descRange[0], 17);
			TexturesRange->Fill(descRange[1], 0);

            CD3DX12_ROOT_PARAMETER1 rootParameters[3];
            rootParameters[1].InitAsConstants(sizeof(BruteForce::Math::Matrix) / 4, 1, 0, D3D12_SHADER_VISIBILITY_ALL);
            rootParameters[0].InitAsConstants(1, 0, 0, D3D12_SHADER_VISIBILITY_ALL);
            rootParameters[2].InitAsDescriptorTable(_countof(descRange), descRange, D3D12_SHADER_VISIBILITY_PIXEL);

            CD3DX12_STATIC_SAMPLER_DESC linearClampSampler(
                0,
                D3D12_FILTER_ANISOTROPIC,//D3D12_FILTER_MIN_MAG_MIP_LINEAR,
                D3D12_TEXTURE_ADDRESS_MODE_WRAP,
                D3D12_TEXTURE_ADDRESS_MODE_WRAP,
                D3D12_TEXTURE_ADDRESS_MODE_WRAP
            );

            CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription;

            rootSignatureDescription.Init_1_1(_countof(rootParameters), rootParameters, 1, &linearClampSampler, rootSignatureFlags);

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
                CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
                CD3DX12_PIPELINE_STATE_STREAM_VS VS;
                CD3DX12_PIPELINE_STATE_STREAM_PS PS;
                CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL DepthStencilState;
                CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
            } pipelineStateStream;

            D3D12_RT_FORMAT_ARRAY rtvFormats = {};
            rtvFormats.NumRenderTargets = 1;
            rtvFormats.RTFormats[0] = desc.RTFormat;
            CD3DX12_DEPTH_STENCIL_DESC depthStencilDesc(D3D12_DEFAULT);
            depthStencilDesc.DepthEnable = false;
            depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
            pipelineStateStream.DepthStencilState = depthStencilDesc;
            pipelineStateStream.pRootSignature = m_RootSignature.Get();
            pipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
            pipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(vertexShaderBlob.Get());
            pipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(pixelShaderBlob.Get());
            pipelineStateStream.RTVFormats = rtvFormats;

            D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = {
                sizeof(PipelineStateStream), &pipelineStateStream
            };
            ThrowIfFailed(device->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&m_PipelineState)));
            m_PipelineState->SetName(L"Sky PSO");
        }

        SmartCommandList& ScreenSpaceSky::PrepareRenderCommandList(SmartCommandList& smart_command_list, const PrepareRenderHelper& render_dest)
        {
            smart_command_list.BeginEvent(0, "RenderSky");
            uint32_t buff_index = render_dest.frame_index;

            const auto& sun_info = GlobalLevelInfo::ReadGlobalAtmosphereInfo();
            m_SkyBuffers[buff_index].m_CpuBuffer->LightDir = sun_info.m_SunInfo;
            m_SkyBuffers[buff_index].m_CpuBuffer->LightColor = sun_info.m_SunColor; 
            m_SkyBuffers[buff_index].m_CpuBuffer->LightColor.w = sun_info.m_SunInfo.w ;

            m_SkyBuffers[buff_index].m_CpuBuffer->MoonDirection = sun_info.m_MoonInfo;
            m_SkyBuffers[buff_index].m_CpuBuffer->MoonColor = sun_info.m_MoonColor;
            m_SkyBuffers[buff_index].m_CpuBuffer->MoonColor.w = sun_info.m_MoonInfo.w;

            m_SkyBuffers[buff_index].m_CpuBuffer->SkyColor = { 0.1f, 0.4f, 0.9f, sun_info.m_SunInfo.w };
            Math::Store(&(m_SkyBuffers[buff_index].m_CpuBuffer->CamPosition), render_dest.camera.GetPosition());
            m_SkyBuffers[buff_index].Update();

            auto& commandList = smart_command_list.command_list;
            smart_command_list.SetPipelineState(m_PipelineState);
            smart_command_list.SetGraphicsRootSignature(m_RootSignature);

            ID3D12DescriptorHeap* ppHeaps[] = { render_dest.HeapManager.GetDescriptorHeapPointer() };
            commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

            commandList->SetGraphicsRootDescriptorTable(2,
                render_dest.HeapManager.GetGpuDescriptorHandle());

            commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            commandList->IASetVertexBuffers(0, 1, NULL);
            commandList->IASetIndexBuffer(NULL);
            commandList->RSSetViewports(1, render_dest.m_Viewport);
            commandList->RSSetScissorRects(1, render_dest.m_ScissorRect);
            commandList->OMSetRenderTargets(1, render_dest.rtv, FALSE, NULL);

            auto const_size = sizeof(BruteForce::Math::Matrix) / 4;
            commandList->SetGraphicsRoot32BitConstants(1, static_cast<UINT>(const_size), render_dest.camera.GetInverseCameraMatrixPointer(), 0);
            commandList->SetGraphicsRoot32BitConstants(0, 1, &buff_index, 0);

            commandList->DrawInstanced(3, 1, 0, 0);
            smart_command_list.EndEvent();
            return smart_command_list;
        }
    }
}
