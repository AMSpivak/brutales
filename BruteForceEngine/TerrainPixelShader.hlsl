struct PixelShaderInput
{
    float4 WorldPosition  : WORLD_POSITION;
    float4 Position : SV_Position;
    float3 Normal : NORMAL;


    nointerpolation uint id: InstanceID;
};


struct FrameInfo
{
    uint frame_index;
};
ConstantBuffer<FrameInfo> FrameInfoCB : register(b0);

#include "TerrainVertexCB.h"

ConstantBuffer<TerrainVertexCB> PlanesCB[3] : register(b17);

Texture2D<uint4> tex_material_id : register(t1);
Texture2D<float4> shadow_tex[3] : register(t2);

Texture2D tex[] : register(t5);
sampler sampl : register(s0);

float4 main(PixelShaderInput IN) : SV_Target
{
    //int material = IN.id;
    const float map_scale = 0.0001f;
//return tex1[material].Sample(sampl, IN.Color.xy);
//return tex[NonUniformResourceIndex(material)].Sample(sampl[1], IN.Color.xy);
    int3 load_pos = int3(0, 0, 0);
    tex_material_id.GetDimensions(load_pos.x, load_pos.y);
    load_pos.xy = round((IN.WorldPosition.xz * map_scale + float2(0.5f, 0.5f)) * load_pos.xy);
    uint4 materials = NonUniformResourceIndex(tex_material_id.Load(load_pos));
    //materials = clamp(materials, uint4(0, 0, 0, 0), uint4(1, 1, 1, 1));
    //uint4 materials = tex_material_id.Sample(sampl[1], (IN.WorldPosition.xz * map_scale + float2(0.5f,0.5f)));
    //return tex[materials.r].Sample(sampl[0], IN.WorldPosition.xz);
    float light_force = 100.0f;
    float diff = 0.03f;
    float light = light_force;// (IN.Normal.y* (1.0f - diff) + diff)* light_force;
    float2 pos = IN.WorldPosition.xz * PlanesCB[FrameInfoCB.frame_index].m_TerrainScaler.xz + float2(0.5f, 0.5f);
    float2 shadows = shadow_tex[FrameInfoCB.frame_index].SampleLevel(sampl, pos, 0).xy;
    //shadows.x = smoothstep(shadows.y, shadows.x, IN.WorldPosition.y);
    shadows.x = smoothstep(shadows.y - 0.01f, shadows.x, IN.WorldPosition.y);
    light *= 0.3 + 0.7 * shadows.x;// *shadows.x;
    return float4(light * tex[materials.r].Sample(sampl, IN.WorldPosition.xz).xyz, 1.0f);
    
    //return float4(0.5f * IN.Normal + float3(0.5f,0.5f,0.5f), 1.0f);
}