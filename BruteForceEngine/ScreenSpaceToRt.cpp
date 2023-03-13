#include "ScreenSpaceToRT.h"
#include "Settings.h"
#include "Helpers.h"
#include "Resources.h"

namespace BruteForce
{
    namespace Render
    {
        ScreenSpaceToRt::ScreenSpaceToRt():m_TonemapBuffers(nullptr)
        {
        }
        ScreenSpaceToRt::~ScreenSpaceToRt()
        {
            if (m_TonemapBuffers)
            {
                delete[] m_TonemapBuffers;
            }

        }

        void ScreenSpaceToRt::PrepareCB(uint32_t index)
        {
            //m_TonemapBuffers[rt_index].m_CpuBuffer->CurveType = m_HDRMode;
            //m_TonemapBuffers[rt_index].m_CpuBuffer->Nits = render_dest.m_Nits;
            //m_TonemapBuffers[rt_index].Update();
        }

        void ScreenSpaceToRt::Update(float delta_time, uint8_t frame_index)
        {
        }

        void ScreenSpaceToRt::LoadContent(Device& device, uint8_t frames_count, const RenderSubsystemInitDesc& desc, SmartCommandQueue& copy_queue, DescriptorHeapManager& descriptor_heap_manager)
        {
            if (m_TonemapBuffers)
            {
                delete[] m_TonemapBuffers;
            }

            m_TonemapBuffers = new ConstantBuffer<TonemapCB>[RenderNumFrames];

            {
                CbvRange = descriptor_heap_manager.AllocateManagedRange(device, static_cast<UINT>(RenderNumFrames), BruteForce::DescriptorRangeTypeCvb, "TonemapCBVs");
                auto& srv_handle = CbvRange->m_CpuHandle;//descriptor_heap_manager.AllocateRange(device, static_cast<UINT>(frames_count), CbvRange);

                for (int i = 0; i < RenderNumFrames; i++)
                {
                    CreateUploadGPUBuffer(device, m_TonemapBuffers[i], srv_handle);

                    m_TonemapBuffers[i].Map();
                    m_TonemapBuffers[i].Update();

                    srv_handle.ptr += device->GetDescriptorHandleIncrementSize(BruteForce::DescriptorHeapCvbSrvUav);
                }
            }

            auto& settings = BruteForce::GetSettings();
            std::wstring content_path{ settings.GetExecuteDirWchar() };

            BruteForce::DataBlob vertexShaderBlob;
            ThrowIfFailed(D3DReadFileToBlob((content_path + L"ScreenSpaceVertexShader.cso").c_str(), &vertexShaderBlob));
            BruteForce::DataBlob pixelShaderBlob;
            ThrowIfFailed(D3DReadFileToBlob((content_path + L"ToneMapPixelShader.cso").c_str(), &pixelShaderBlob));

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

            CD3DX12_DESCRIPTOR_RANGE1 descRange[3];

            RTSrvDescriptors = descriptor_heap_manager.GetManagedRange("RenderTargetsSrvs");
            assert(RTSrvDescriptors);

            RTSrvDescriptors->Fill(descRange[1], 1);

            CbvRange->Fill(descRange[2], 2);

            std::shared_ptr<DescriptorHeapRange> LuminanceSrvDescriptors = descriptor_heap_manager.GetManagedRange("LuminanceSrvs");
            LuminanceSrvDescriptors->Fill(descRange[0], 1,1, 0);

            //descRange.NumDescriptors = 1;

            CD3DX12_ROOT_PARAMETER1 rootParameters[2];
            rootParameters[0].InitAsDescriptorTable(_countof(descRange), descRange, D3D12_SHADER_VISIBILITY_PIXEL);
            rootParameters[1].InitAsConstants(1, 0, 0, D3D12_SHADER_VISIBILITY_PIXEL);

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

            LPCWSTR PSONames[] = { L"Tone mapping R8G8B8A8 PSO" ,L"Tone mapping R10G10B10A2 PSO" ,L"Tone mapping R16G16B16A16 PSO" };
            TargetFormat m_swapChainFormats[] = { TargetFormat_R8G8B8A8_Unorm, TargetFormat_R10G10B10A2_Unorm, TargetFormat_R16G16B16A16_Float };
            for(int i = 0; i < NumPSO; i++)
            {
                rtvFormats.RTFormats[0] = m_swapChainFormats[i];
                pipelineStateStream.RTVFormats = rtvFormats;

                ThrowIfFailed(device->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&m_PipelineStates[i])));
                m_PipelineStates[i]->SetName(PSONames[i]);
            }
        }

        SmartCommandList& ScreenSpaceToRt::PrepareRenderCommandList(SmartCommandList& smart_command_list, const PrepareRenderHelper& render_dest)
        {
            smart_command_list.BeginEvent(0, "ToneMapping");
            auto& commandList = smart_command_list.command_list;
            smart_command_list.SetPipelineState(m_PipelineStates[m_HDRMode]);
            smart_command_list.SetGraphicsRootSignature(m_RootSignature);

            ID3D12DescriptorHeap* ppHeaps[] = { render_dest.HeapManager.GetDescriptorHeapPointer()/*, m_SamplerHeap.Get()*/};
            commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

            
            uint32_t rt_index = render_dest.rt_index;
            PrepareCB(rt_index);
            m_TonemapBuffers[rt_index].m_CpuBuffer->CurveType = m_HDRMode;
            m_TonemapBuffers[rt_index].m_CpuBuffer->Nits = render_dest.m_Nits;
            m_TonemapBuffers[rt_index].Update();

            commandList->SetGraphicsRoot32BitConstants(1, 1, &rt_index, 0);
            commandList->SetGraphicsRootDescriptorTable(0, render_dest.HeapManager.GetGpuDescriptorHandle());

            commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            commandList->IASetVertexBuffers(0, 1, NULL);
            commandList->IASetIndexBuffer(NULL);
            commandList->RSSetViewports(1, render_dest.m_Viewport);
            commandList->RSSetScissorRects(1, render_dest.m_ScissorRect);
            commandList->OMSetRenderTargets(1, render_dest.rtv, FALSE, NULL);

            commandList->DrawInstanced(3, 1, 0, 0);
            smart_command_list.EndEvent();
            return smart_command_list;
        }
        void ScreenSpaceToRt::SetHDRMode(HDRMode::HDRMode mode)
        {
            m_HDRMode = mode;
        }
    }
}