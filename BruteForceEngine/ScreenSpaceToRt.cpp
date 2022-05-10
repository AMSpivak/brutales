#include "ScreenSpaceToRT.h"
#include "Settings.h"
#include "Helpers.h"
#include "Resources.h"

namespace BruteForce
{
    namespace Render
    {
        ScreenSpaceToRt::ScreenSpaceToRt()
        {
        }
        ScreenSpaceToRt::~ScreenSpaceToRt()
        {
        }
        void ScreenSpaceToRt::Update(float delta_time, uint8_t frame_index)
        {
        }
        void ScreenSpaceToRt::LoadContent(Device& device, uint8_t frames_count, const RenderSubsystemInitDesc& desc)
        {
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
                samplerDesc.Filter = D3D12_FILTER_ANISOTROPIC;// D3D12_FILTER_MIN_MAG_MIP_LINEAR;
                samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
                samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
                samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
                samplerDesc.MinLOD = 0;
                samplerDesc.MaxLOD = 0.0;
                samplerDesc.MipLODBias = 0.0f;
                samplerDesc.MaxAnisotropy = 1;
                samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
                auto sampler_handle = m_SamplerHeap->GetCPUDescriptorHandleForHeapStart();
                device->CreateSampler(&samplerDesc, sampler_handle);
            }

            BruteForce::DataBlob vertexShaderBlob;
            ThrowIfFailed(D3DReadFileToBlob((content_path + L"ScreenSpaceVertexShader.cso").c_str(), &vertexShaderBlob));
            BruteForce::DataBlob pixelShaderBlob;
            ThrowIfFailed(D3DReadFileToBlob((content_path + L"ToneMapPixelShader.cso").c_str(), &pixelShaderBlob));

            D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
                { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
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

            CD3DX12_DESCRIPTOR_RANGE1 descRange;
            descRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
            //descRange.OffsetInDescriptorsFromTableStart = 0;

            CD3DX12_DESCRIPTOR_RANGE1 descRangeSamp;
            descRangeSamp.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 1, 0);

            CD3DX12_ROOT_PARAMETER1 rootParameters[2];
            //rootParameters[2].InitAsConstants(1, 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);
            rootParameters[0].InitAsDescriptorTable(1, &descRange, D3D12_SHADER_VISIBILITY_PIXEL);
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


            struct PipelineStateStream
            {
                CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE pRootSignature;
                CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
                CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
                CD3DX12_PIPELINE_STATE_STREAM_VS VS;
                CD3DX12_PIPELINE_STATE_STREAM_PS PS;
                CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL DepthStencilState;
                //CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
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
            pipelineStateStream.InputLayout = { inputLayout, _countof(inputLayout) };
            pipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
            pipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(vertexShaderBlob.Get());
            pipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(pixelShaderBlob.Get());
            //pipelineStateStream.DSVFormat = desc.DepthFormat;
            pipelineStateStream.RTVFormats = rtvFormats;

            D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = {
                sizeof(PipelineStateStream), &pipelineStateStream
            };
            ThrowIfFailed(device->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&m_PipelineState)));
            m_PipelineState->SetName(L"Tone mapping PSO");


            size_t num_indexes = 3;
            WORD* indexes = new WORD[num_indexes]{0,2,1};

            SmartCommandQueue smart_queue(device, BruteForce::CommandListTypeDirect);
            auto commandList = smart_queue.GetCommandList();
            BruteForce::pResource intermediateIndexBuffer;
            BruteForce::UpdateBufferResource(device, commandList,
                &m_IndexBuffer, &intermediateIndexBuffer,
                num_indexes, sizeof(WORD), indexes);

            m_IndexBufferView.BufferLocation = m_IndexBuffer->GetGPUVirtualAddress();
            m_IndexBufferView.Format = DXGI_FORMAT_R16_UINT;
            m_IndexBufferView.SizeInBytes = static_cast<UINT>(sizeof(WORD) * num_indexes);


            auto fenceValue = smart_queue.ExecuteCommandList(commandList);
            smart_queue.WaitForFenceValue(fenceValue);
            intermediateIndexBuffer->Release();
        }

        SmartCommandList& ScreenSpaceToRt::PrepareRenderCommandList(SmartCommandList& smart_command_list, const RenderDestination& render_dest)
        {
            // TODO: вставьте здесь оператор return
            //uint32_t buff_index = render_dest.frame_index;

            auto& commandList = smart_command_list.command_list;
            smart_command_list.SetPipelineState(m_PipelineState);
            smart_command_list.SetRootSignature(m_RootSignature);

            ID3D12DescriptorHeap* ppHeaps[] = { m_SrvHeapPtr.Get(), m_SamplerHeap.Get()};
            commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

            commandList->SetGraphicsRootDescriptorTable(0,
                m_SrvHeapPtr->GetGPUDescriptorHandleForHeapStart());
            commandList->SetGraphicsRootDescriptorTable(1,
                m_SamplerHeap->GetGPUDescriptorHandleForHeapStart());

            commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            commandList->IASetVertexBuffers(0, 1, NULL);
            commandList->IASetIndexBuffer(&m_IndexBufferView);
            commandList->RSSetViewports(1, render_dest.m_Viewport);
            commandList->RSSetScissorRects(1, render_dest.m_ScissorRect);
            commandList->OMSetRenderTargets(1, render_dest.rtv, FALSE, NULL);

            commandList->DrawIndexedInstanced(3, 1, 0, 0, 0);;
            return smart_command_list;
        }

        void ScreenSpaceToRt::SetRenderParameter(DescriptorHeap SrvPtr)
        {
            m_SrvHeapPtr = SrvPtr;
        }
    }
}