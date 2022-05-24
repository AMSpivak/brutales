#include "CalcTerrainShadow.h"
#include "Settings.h"
namespace BruteForce
{
    namespace Render
    {
        CalcTerrainShadow::CalcTerrainShadow() :m_TerrainShadowBuffers(nullptr)
        {
        }

        CalcTerrainShadow::~CalcTerrainShadow()
        {
            if (m_TerrainShadowBuffers)
            {
                delete[] m_TerrainShadowBuffers;
            }
        }
        void CalcTerrainShadow::Update(float delta_time, uint8_t frame_index)
        {
        }
        void CalcTerrainShadow::LoadContent(Device& device, uint8_t frames_count)
        {
            
            if (m_TerrainShadowBuffers)
            {
                delete[] m_TerrainShadowBuffers;
            }

            m_TerrainShadowBuffers = new ConstantBuffer<TerrainShadowCB>[frames_count];

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

            CD3DX12_DESCRIPTOR_RANGE1 srcMip(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);
            CD3DX12_DESCRIPTOR_RANGE1 outMip(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DESCRIPTORS_VOLATILE);

            CD3DX12_ROOT_PARAMETER1 rootParameters[3];
            rootParameters[0].InitAsConstants(sizeof(TerrainShadowCB) / 4, 0);
            rootParameters[1].InitAsDescriptorTable(1, &srcMip);
            rootParameters[2].InitAsDescriptorTable(1, &outMip);

            CD3DX12_STATIC_SAMPLER_DESC linearClampSampler(
                0,
                D3D12_FILTER_MIN_MAG_MIP_LINEAR,
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
        }
        SmartCommandList& CalcTerrainShadow::PrepareRenderCommandList(SmartCommandList& smart_command_list)
        {

            // TODO: insert return statement here
            return smart_command_list;
        }
    }
}
