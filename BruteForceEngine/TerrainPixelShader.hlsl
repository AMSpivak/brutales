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
sampler sampl : register(s0);

struct PS_OUTPUT
{
    float4 Color: SV_Target0;
    float4 Normal: SV_Target1;
    uint4 Material: SV_Target2;
    float4 TexUV: SV_Target3;
    float4 TexDdxDdy: SV_Target4;
};

PS_OUTPUT main(PixelShaderInput IN)// : SV_Target
{
    //int material = IN.id;
    const uint material_offset = 2;
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
    
    float diff = 0.03f;
    float4 sun_info = PlanesCB[FrameInfoCB.frame_index].m_SunInfo;
    float3 Normal_smpl = tex[materials.r * material_offset + 1].Sample(sampl, IN.WorldPosition.xz).xyz;
    //float3 Normal = IN.Normal;
    float3 face_Normal = normalize(IN.Normal);
    float3 B_Normal = normalize(cross(IN.Tangent,IN.Normal ));
    float3 T_Normal = normalize(cross( B_Normal,IN.Normal ));

    matrix TBN2 = transpose(matrix(float4(T_Normal, 0), float4(B_Normal, 0), float4(face_Normal, 0), float4(0, 0, 0, 1)));
    //matrix TBN2 = matrix(float4(1, 0, 0, 0), float4(0, 1, 0, 0), float4(0, 0, 1, 0), float4(0,0,0, 1));

    float4 quat_xm;
    quat_cast_xm(TBN2, quat_xm);
    matrix TBN4;
    mat_cast_xm(quat_xm, TBN4);
    //float3 Normal = Normal_smpl.x * T_Normal + Normal_smpl.z * face_Normal + Normal_smpl.y * B_Normal;
    Normal_smpl.y = - Normal_smpl.y;
    float3 Normal = mul(TBN4, float4(Normal_smpl, 0.0f)).xyz;
    float light_diffuse = clamp(dot(Normal, normalize(sun_info.xyz)),0.0, 1.0);
    float light = sun_info.w;// (IN.Normal.y* (1.0f - diff) + diff)* light_force;
    float2 l_dir = PlanesCB[FrameInfoCB.frame_index].m_SunShadow.xy;
    //float2 pos = IN.WorldPosition.xz * PlanesCB[FrameInfoCB.frame_index].m_TerrainScaler.xz + float2(0.5f, 0.5f);
    float2 pos = IN.WorldPosition.xz * PlanesCB[FrameInfoCB.frame_index].m_TerrainScaler.xz;
    float2 UV = pos + float2(l_dir.x + l_dir.y, l_dir.x - l_dir.y) * 0.5;
    UV = l_dir * UV.x + float2(-l_dir.y, l_dir.x) * UV.y;
    UV = float2(0.5f, 0.5f) + (UV - float2(0.5f, 0.5f)) * PlanesCB[FrameInfoCB.frame_index].m_SunShadow.z;
    float4 shadows = shadow_tex[FrameInfoCB.frame_index].SampleLevel(sampl, UV, 0);
    //shadows.x = (IN.WorldPosition.y - shadows.w) / (0.001 + shadows.x - shadows.y); //smoothstep(shadows.y, shadows.x, IN.WorldPosition.y);
    //shadows.x = (- shadows.w)/(0.001 + shadows.x - shadows.y); //smoothstep(shadows.y, shadows.x, IN.WorldPosition.y);
    //shadows.x = clamp(shadows.x, 0, 1.0f);
    shadows.x = smoothstep(shadows.y, shadows.x, shadows.z);// IN.WorldPosition.y);
    //shadows.x = 1.0 - shadows.x * shadows.x;
    light *= 0.3 + 0.7 * light_diffuse * shadows.x;// *shadows.x;
    
    //float3 Color = tex[materials.r * material_offset].Sample(sampl, IN.WorldPosition.xz).xyz;
    float2 derivX = ddx(IN.WorldPosition.xz);
    float2 derivY = ddy(IN.WorldPosition.xz);

    float3 Color = tex[materials.r * material_offset].SampleGrad(sampl, IN.WorldPosition.xz, derivX, derivY).xyz;
    Color = pow(Color, 2.2);

    PS_OUTPUT output;
    output.Color = float4(light * Color * PlanesCB[FrameInfoCB.frame_index].m_SunColor.xyz, 1.0f);//Set first output

    output.Normal = quat_xm;//Set second output
    output.Material = materials;
    output.TexDdxDdy = float4(derivX, derivY);
    output.TexUV = float4(IN.WorldPosition.xz, 0.0, 0.0);
    return output;
    //return 
    //return float4(100.0f * (IN.Normal), 1.0f);
    
    //return float4(0.5f * IN.Normal + float3(0.5f,0.5f,0.5f), 1.0f);
}