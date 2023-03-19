#include "ComputeLuminanceCB.h"



ConstantBuffer<ComputeLuminanceCB> LuminanceCB : register(b0);

Texture2D<float4> SrcLuminance : register(t0);

RWTexture2D<float> OutLuminance[2] : register(u0);
SamplerState LinearClampSampler : register(s0);

#include "CommonComputeInput.h"

#define BLOCK_SIZE 16
#define STEPS 8
groupshared float gs_Luminance[BLOCK_SIZE* BLOCK_SIZE];

[numthreads(BLOCK_SIZE, BLOCK_SIZE, 1)]
void main(ComputeShaderInput IN)
{
    uint w = 0;
    uint h = 0;
    SrcLuminance.GetDimensions(w, h);
    float TexelSize = 2.0 / w;
    float2 UV = TexelSize * (IN.DispatchThreadID.xy + 0.5);

    float src = SrcLuminance.SampleLevel(LinearClampSampler, UV, 0).r;
    gs_Luminance[IN.GroupIndex] = src;
    uint index = IN.GroupIndex;
    uint offset = 1;
    for (uint i = 0; i < STEPS; i++)
    {
        GroupMemoryBarrierWithGroupSync();
        if ((index & 1) == 0)
        {
            src += gs_Luminance[IN.GroupIndex + offset];
            src *= 0.5;
            gs_Luminance[IN.GroupIndex] = src;
        }
        index = index >> 1;
        offset = offset << 1;
    }
    GroupMemoryBarrierWithGroupSync();

    if(IN.GroupIndex == 0)
    {
        OutLuminance[0][int2(IN.GroupID.xy)] = gs_Luminance[IN.GroupIndex];// src;
    }
    //GroupMemoryBarrierWithGroupSync();
}