#include "ComputeLuminanceCB.h"



ConstantBuffer<ComputeLuminanceCB> LuminanceCB : register(b0);

Texture2D<float4> SrcLuminance : register(t0);

RWTexture2D<float> OutLuminance[2] : register(u0);
SamplerState LinearClampSampler : register(s0);

struct ComputeShaderInput
{
    uint3 GroupID           : SV_GroupID;           // 3D index of the thread group in the dispatch.
    uint3 GroupThreadID     : SV_GroupThreadID;     // 3D index of local thread ID in a thread group.
    uint3 DispatchThreadID  : SV_DispatchThreadID;  // 3D index of global thread ID in the dispatch.
    uint  GroupIndex        : SV_GroupIndex;        // Flattened local index of the thread within a thread group.
};

#define BLOCK_SIZE 16

groupshared float gs_Luminance[BLOCK_SIZE* BLOCK_SIZE];

[numthreads(BLOCK_SIZE, BLOCK_SIZE, 1)]
void main(ComputeShaderInput IN)
{
    float TexelSize = 1.0 / LuminanceCB.srcTextureSize;
    float2 UV = TexelSize * (IN.DispatchThreadID.xy + 0.5);

    float src = SrcLuminance.SampleLevel(LinearClampSampler, UV, 0).r;

    OutLuminance[0][int2(IN.DispatchThreadID.xy)] = 100;// src;
    gs_Luminance[IN.GroupIndex] = src;
}