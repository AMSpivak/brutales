#include "math.hlsli"

struct PixelShaderInput
{
    float4 WorldPosition  : WORLD_POSITION;
    float4 Position : SV_Position;
    float3 Normal : NORMAL;
    float3 Tangent : T_NORMAL;


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
SamplerState sampl : register(s0);

struct PS_OUTPUT
{
    float4 Normal: SV_Target0;
    uint4 Material: SV_Target1;
    float4 TexUV: SV_Target2;
    float4 TexDdxDdy: SV_Target3;
};

PS_OUTPUT main(PixelShaderInput IN)// : SV_Target
{
    const uint material_offset = 2;
    const float map_scale = 0.0001f;
    int3 load_pos = int3(0, 0, 0);
    tex_material_id.GetDimensions(load_pos.x, load_pos.y);
    load_pos.xy = round((IN.WorldPosition.xz * map_scale + float2(0.5f, 0.5f)) * load_pos.xy);
    uint4 materials = NonUniformResourceIndex(tex_material_id.Load(load_pos));
    

    float3 face_Normal = normalize(IN.Normal);
    float3 B_Normal = normalize(cross(IN.Tangent,IN.Normal ));
    float3 T_Normal = normalize(cross( B_Normal,IN.Normal ));

    matrix TBN2 = transpose(matrix(float4(-B_Normal, 0), float4(T_Normal, 0), float4(face_Normal, 0), float4(0, 0, 0, 1)));
    float4 quat_xm;
    quat_cast_xm(TBN2, quat_xm);
    float2 derivX = ddx(IN.WorldPosition.xz);
    float2 derivY = ddy(IN.WorldPosition.xz);

    PS_OUTPUT output;

    output.Normal = quat_xm;//Set second output
    output.Material = materials + 1;
    output.TexDdxDdy = float4(derivX, derivY);
    output.TexUV = float4((IN.WorldPosition.xz), 0.0, 0.0);
    return output;
}