
//#define GenerateMips_RootSignature \
//    "RootFlags(0), " \
//    "RootConstants(b0, num32BitConstants = 2), " \
//    "DescriptorTable( SRV(t0, numDescriptors = 1) )," \
//    "DescriptorTable( UAV(u0, numDescriptors = 1) )," \
//    "StaticSampler(s0," \
//        "addressU = TEXTURE_ADDRESS_CLAMP," \
//        "addressV = TEXTURE_ADDRESS_CLAMP," \
//        "addressW = TEXTURE_ADDRESS_CLAMP," \
//        "filter = FILTER_MIN_MAG_MIP_LINEAR)"

#include "TerrainShadowCB.h"


struct FrameInfo
{
    uint frame_index;
};
ConstantBuffer<FrameInfo> FrameInfoCB : register(b0);

ConstantBuffer<TerrainShadowCB> terrain_shadowCB[3] : register(b1);
 
Texture2D<float4> SrcTerrain : register(t0);

RWTexture2D<float4> OutShadow[3] : register(u0);
SamplerState TerrainSampler : register(s0);

#define BLOCK_SIZE 64
[numthreads(BLOCK_SIZE, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{

    const float shadow_height_decrease = terrain_shadowCB[FrameInfoCB.frame_index].LightSpace2.z;
    const float shadow_height_decrease_second = terrain_shadowCB[FrameInfoCB.frame_index].LightSpace2.w;
    const float shadow_height_decrease_moon = terrain_shadowCB[FrameInfoCB.frame_index].LightSpace2_moon.z;
    const float shadow_height_decrease_second_moon = terrain_shadowCB[FrameInfoCB.frame_index].LightSpace2_moon.w;
    //OutShadow[int2(DTid.x, DTid.y)] = float4(1, 1, 1.0f, 0.0f);

    int row = DTid.x;
    if (row < terrain_shadowCB[FrameInfoCB.frame_index].srcTextureSize.y)
    {
        float2 l_dir = terrain_shadowCB[FrameInfoCB.frame_index].LightSpace1.xy / terrain_shadowCB[FrameInfoCB.frame_index].srcTextureSize.x;

        float shadow_height = 0;
        int shadow_index = -1;

        float shadow_height_second = 0;
        int shadow_index_second = -1;

        float2 l_dir_moon = terrain_shadowCB[FrameInfoCB.frame_index].LightSpace1_moon.xy / terrain_shadowCB[FrameInfoCB.frame_index].srcTextureSize.x;

        float shadow_height_moon = 0;
        int shadow_index_moon = -1;

        float shadow_height_second_moon = 0;
        int shadow_index_second_moon = -1;

        //const float h_offset = 0.01;
        //float h_old = 0.0f;
        for (int i = 0; i < terrain_shadowCB[FrameInfoCB.frame_index].srcTextureSize.x; i++)
        {
            float2 UV = l_dir * i + float2(-l_dir.y, l_dir.x) * row;// +terrain_shadowCB[FrameInfoCB.frame_index].LightSpace1.zw;
            UV -= float2(terrain_shadowCB[FrameInfoCB.frame_index].LightSpace1.x  - terrain_shadowCB[FrameInfoCB.frame_index].LightSpace1.y,
                          terrain_shadowCB[FrameInfoCB.frame_index].LightSpace1.y +terrain_shadowCB[FrameInfoCB.frame_index].LightSpace1.x) * 0.5f;// terrain_shadowCB[FrameInfoCB.frame_index].LightSpace1.zw;
            UV = float2(0.5f, 0.5f) + terrain_shadowCB[FrameInfoCB.frame_index].LightSpace1.z * UV;
            float h = SrcTerrain.SampleLevel(TerrainSampler, UV, 0).r * terrain_shadowCB[FrameInfoCB.frame_index].LightSpace2.y;
            float calc_h = (shadow_height - (i - shadow_index) * shadow_height_decrease);
            if (h > calc_h)
            {
                shadow_height = h;
                shadow_index = i;
            }

            float calc_h_second = (shadow_height_second - (i - shadow_index_second) * shadow_height_decrease_second);
            if (h > calc_h_second)
            {
                shadow_height_second = h;
                shadow_index_second = i;
            }
            OutShadow[0][int2(i, row)] = float4(calc_h, calc_h_second, h, calc_h_second - h);

            UV = l_dir_moon * i + float2(-l_dir_moon.y, l_dir_moon.x) * row;// +terrain_shadowCB[FrameInfoCB.frame_index].LightSpace1.zw;
            UV -= float2(terrain_shadowCB[FrameInfoCB.frame_index].LightSpace1_moon.x - terrain_shadowCB[FrameInfoCB.frame_index].LightSpace1_moon.y,
                terrain_shadowCB[FrameInfoCB.frame_index].LightSpace1_moon.y + terrain_shadowCB[FrameInfoCB.frame_index].LightSpace1_moon.x) * 0.5f;// terrain_shadowCB[FrameInfoCB.frame_index].LightSpace1.zw;
            UV = float2(0.5f, 0.5f) + terrain_shadowCB[FrameInfoCB.frame_index].LightSpace1_moon.z * UV;

            h = SrcTerrain.SampleLevel(TerrainSampler, UV, 0).r * terrain_shadowCB[FrameInfoCB.frame_index].LightSpace2_moon.y;
            calc_h = (shadow_height_moon - (i - shadow_index_moon) * shadow_height_decrease_moon);
            if (h > calc_h)
            {
                shadow_height_moon = h;
                shadow_index_moon = i;
            }

            calc_h_second = (shadow_height_second_moon - (i - shadow_index_second_moon) * shadow_height_decrease_second_moon);
            if (h > calc_h_second)
            {
                shadow_height_second_moon = h;
                shadow_index_second_moon = i;
            }

            OutShadow[1][int2(i, row)] = float4(calc_h, calc_h_second, h, calc_h_second - h);
        }
    }

}