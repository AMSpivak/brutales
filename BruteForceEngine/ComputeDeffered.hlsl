#include "DefferedLightingCB.h"
#include "CommonComputeInput.h"
#include "math.hlsli"


struct FrameInfo
{
    uint frame_index;
};
ConstantBuffer<FrameInfo> FrameInfoCB : register(b0);

ConstantBuffer<DefferedLightingCB> lighting_CB[2] : register(b2);

//Texture2D<uint4> tex_material_id : register(t1);
Texture2D<float4> shadow_tex[3] : register(t0);

Texture2D<float4> TBN_Quat_tex : register(t3);
Texture2D<uint4 > Material_tex : register(t4);
Texture2D<float4> UV_tex : register(t5);
Texture2D<float4> Ddx_Ddy_tex : register(t6);


Texture2D<float4> textures[] : register(t9);
Texture2D<float> depth : register(t8);
RWTexture2D<float4> OutImage[2] : register(u0);

sampler sampl : register(s0);


#define BLOCK_SIZE 8
[numthreads(BLOCK_SIZE, BLOCK_SIZE, 1)]
void main(ComputeShaderInput IN)
{
    uint w = 0;
    uint h = 0;
    TBN_Quat_tex.GetDimensions(w, h);
    if(IN.DispatchThreadID.x < w && IN.DispatchThreadID.y < h)
    {
        //if (IN.DispatchThreadID.x & 1)
        {
            float4 quat = TBN_Quat_tex.Load(IN.DispatchThreadID.xyz);
            uint4 materials = (Material_tex.Load(IN.DispatchThreadID.xyz));
            if (materials.r == 0)
                return;
            materials -= 1;
            const uint material_offset = 2;

            //uint4 materials = NonUniformResourceIndex(Material_tex.Load(IN.DispatchThreadID.xyz));
            float4 UV = UV_tex.Load(int3(IN.DispatchThreadID.xy,0));
            float4 Ddx_Ddy = Ddx_Ddy_tex.Load(IN.DispatchThreadID.xyz);
            float depth_val = depth.Load(IN.DispatchThreadID.xyz).r;// *2.0 - 1.0;
            float4 screen_pos = float4(IN.DispatchThreadID.xy, depth_val, 1);
            screen_pos.x = (screen_pos.x / w) * 2.0 - 1.0;
            screen_pos.y = (screen_pos.y / h) * 2.0 - 1.0;
            screen_pos.y *= -1.0;
            float4 world_pos = mul(lighting_CB[FrameInfoCB.frame_index].m_CameraInverse, screen_pos);
            world_pos /= world_pos.w;
            world_pos.xyz += lighting_CB[FrameInfoCB.frame_index].m_CameraPosition.xyz;

            float2 l_dir = lighting_CB[FrameInfoCB.frame_index].m_SunShadow.xy;

            float2 pos = world_pos.xz * lighting_CB[FrameInfoCB.frame_index].m_TerrainScaler.xz;
            float2 shadowUV = pos + float2(l_dir.x + l_dir.y, l_dir.x - l_dir.y) * 0.5;
            shadowUV = l_dir * shadowUV.x + float2(-l_dir.y, l_dir.x) * shadowUV.y;
            shadowUV = float2(0.5f, 0.5f) + (shadowUV - float2(0.5f, 0.5f)) * lighting_CB[FrameInfoCB.frame_index].m_SunShadow.z;

            float4 shadows = shadow_tex[FrameInfoCB.frame_index].SampleLevel(sampl, shadowUV, 0);
            shadows.x = smoothstep(shadows.y, shadows.x, shadows.z);
            matrix TBN;
            mat_cast_xm(quat, TBN);
            //float3 Normal = Normal_smpl.x * T_Normal + Normal_smpl.z * face_Normal + Normal_smpl.y * B_Normal;
            //Normal_smpl.y = -Normal_smpl.y;
            float3 Normal_smpl = textures[materials.r * material_offset + 1].SampleGrad(sampl, UV.xy, Ddx_Ddy.xy, Ddx_Ddy.zw).xyz * 2.0 -1.0;
            //Normal_smpl.y = -Normal_smpl.y;
            float3 Normal = mul(TBN, float4(Normal_smpl, 0.0f)).xyz;
            //float3 Normal = mul(TBN, float4(0.0,0.0,1.0 ,0.0f)).xyz;

            float4 sun_info = lighting_CB[FrameInfoCB.frame_index].m_SunInfo;
            float4 moon_info = lighting_CB[FrameInfoCB.frame_index].m_MoonInfo;

            float sun_light_diffuse = dot(Normal, normalize(sun_info.xyz));
            float sun_light = sun_info.w;
            float moon_light_diffuse = dot(Normal, normalize(moon_info.xyz));
            float moon_light = moon_info.w;

            sun_light *= 0.2 + clamp(sun_light_diffuse, 0.0, 1.0) * shadows.x;// *shadows.x;// *shadows.x;
            float sun_intens = smoothstep(-0.1, -0.0, sun_info.y);
            sun_light *= sun_intens * sun_intens * sun_intens;
            moon_light *= 0.2 + clamp(moon_light_diffuse, 0.0, 1.0);// *shadows.x;// *shadows.x;
            moon_light *= smoothstep(-0.3, -0.0, moon_info.y);// *shadows.x;// *shadows.x;

            sun_light *= lighting_CB[FrameInfoCB.frame_index].m_SunColor;

            float3 Color = textures[materials.r * material_offset].SampleGrad(sampl, UV.xy, Ddx_Ddy.xy, Ddx_Ddy.zw).xyz;
            Color = pow(Color, 2.2);

            float3 res = sun_light * Color + moon_light * Color;
            //Color *= 0.01;
            //Color = pow(Color, 2.2);
            
            //OutImage[FrameInfoCB.frame_index][IN.DispatchThreadID.xy] = float4(world_pos.xyz, 1.0);
            OutImage[FrameInfoCB.frame_index][IN.DispatchThreadID.xy] = float4(res, 1.0);
        }
    }
}