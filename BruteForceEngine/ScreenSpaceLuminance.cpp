#include "ScreenSpaceLuminance.h"
#include "Settings.h"

namespace BruteForce
{
    namespace Render
    {
        ScreenSpaceLuminance::~ScreenSpaceLuminance()
        {
        }
        void ScreenSpaceLuminance::Update(float delta_time, uint8_t frame_index)
        {
        }

        void ScreenSpaceLuminance::LoadContent(Device& device, uint8_t frames_count, const RenderSubsystemInitDesc& desc, SmartCommandQueue& copy_queue, DescriptorHeapManager& descriptor_heap_manager)
        {


            auto& settings = BruteForce::GetSettings();
            std::wstring content_path{ settings.GetExecuteDirWchar() };

            BruteForce::DataBlob vertexShaderBlob;
            ThrowIfFailed(D3DReadFileToBlob((content_path + L"ScreenSpaceVertexShader.cso").c_str(), &vertexShaderBlob));
            BruteForce::DataBlob pixelShaderBlob;
            ThrowIfFailed(D3DReadFileToBlob((content_path + L"Luminance1Pass.cso").c_str(), &pixelShaderBlob));

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

            DescriptorRange descRange;
            RTSrvDescriptors = descriptor_heap_manager.GetManagedRange("RenderTargetsSrvs");
            assert(RTSrvDescriptors);
            RTSrvDescriptors->Fill(descRange, 0);

            CD3DX12_ROOT_PARAMETER1 rootParameters;
            rootParameters.InitAsDescriptorTable(1, &descRange, D3D12_SHADER_VISIBILITY_PIXEL);

            CD3DX12_STATIC_SAMPLER_DESC linearClampSampler(
                0,
                D3D12_FILTER_ANISOTROPIC,//D3D12_FILTER_MIN_MAG_MIP_LINEAR,
                D3D12_TEXTURE_ADDRESS_MODE_WRAP,
                D3D12_TEXTURE_ADDRESS_MODE_WRAP,
                D3D12_TEXTURE_ADDRESS_MODE_WRAP
            );

            CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDescription;

            rootSignatureDescription.Init_1_1(1, &rootParameters, 1, &linearClampSampler, rootSignatureFlags);

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

        SmartCommandList& ScreenSpaceLuminance::PrepareRenderCommandList(SmartCommandList& smart_command_list, const PrepareRenderHelper& render_dest)
        {
            smart_command_list.BeginEvent(0, "Luminance");
            uint32_t buff_index = render_dest.frame_index;

            auto& commandList = smart_command_list.command_list;
            smart_command_list.SetPipelineState(m_PipelineState);
            smart_command_list.SetGraphicsRootSignature(m_RootSignature);

            ID3D12DescriptorHeap* ppHeaps[] = { render_dest.HeapManager.GetDescriptorHeapPointer() };
            commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

            commandList->SetGraphicsRootDescriptorTable(0,
                render_dest.HeapManager.GetGpuDescriptorHandle());

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
    }
}
