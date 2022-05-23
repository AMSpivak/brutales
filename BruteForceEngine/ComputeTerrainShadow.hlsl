
#define GenerateMips_RootSignature \
    "RootFlags(0), " \
    "RootConstants(b0, num32BitConstants = 2), " \
    "DescriptorTable( SRV(t0, numDescriptors = 1) )," \
    "DescriptorTable( UAV(u0, numDescriptors = 1) )," \
    "StaticSampler(s0," \
        "addressU = TEXTURE_ADDRESS_CLAMP," \
        "addressV = TEXTURE_ADDRESS_CLAMP," \
        "addressW = TEXTURE_ADDRESS_CLAMP," \
        "filter = FILTER_MIN_MAG_MIP_LINEAR)"

#include "TerrainShadow.h"

// Source mip map.
Texture2D<float4> SrcTerrain : register(t0);

// Write up to 4 mip map levels.
RWTexture2D<float4> OutShadow : register(u0);

SamplerState LinearClampSampler : register(s0);

#define BLOCK_SIZE 64
[numthreads(BLOCK_SIZE, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint t_width;
    uint t_height;

    SrcTerrain.GetDimensions(t_width, t_height);
    float2 texel_size = float2(1.0f / t_width, 1.0f / t_height);

    const float shadow_height_decrease = 1.0f;
    const float shadow_height_decrease_second = 1.1f;

    int row = DTid.x;
    if (row < srcTextureSize.y)
    {
        float shadow_height = 0;
        int shadow_index = -1;

        float shadow_height_second = 0;
        int shadow_index_second = -1;

        for (int i = 0; i < srcTextureSize.x; i++)
        {
            float2 UV = texel_size * float2(i, row);
            float h = SrcTerrain.SampleLevel(LinearClampSampler, UV, 0).r;
            float calc_h = (shadow_height - (i - shadow_index) * shadow_height_decrease);
            if (h > calc_h)
            {
                shadow_height = h;
                shadow_index = i;
                calc_h = h;
            }

            float calc_h_second = (shadow_height - (i - shadow_index) * shadow_height_decrease);
            if (h > calc_h_second)
            {
                shadow_height_second = h;
                shadow_index_second = i;
                calc_h_second = h;
            }

            OutShadow[int2(i, row)] = float4(calc_h, calc_h_second,0.0f,0.0f);
        }
    }

}