#include "DefferedLightingCB.h"
#include "CommonComputeInput.h"
#include "math.hlsli"
#include "LightScattering.hlsli"


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
            float4 sun_info = lighting_CB[FrameInfoCB.frame_index].m_SunInfo;
            float4 moon_info = lighting_CB[FrameInfoCB.frame_index].m_MoonInfo;

            float4 quat = TBN_Quat_tex.Load(IN.DispatchThreadID.xyz);

            float depth_val = depth.Load(IN.DispatchThreadID.xyz).r;// *2.0 - 1.0;
            float4 screen_pos = float4(IN.DispatchThreadID.xy, depth_val, 1);
            screen_pos.x = (screen_pos.x / w) * 2.0 - 1.0;
            screen_pos.y = (screen_pos.y / h) * 2.0 - 1.0;
            screen_pos.y *= -1.0;
            float4 world_pos = mul(lighting_CB[FrameInfoCB.frame_index].m_CameraInverse, screen_pos);
            world_pos /= world_pos.w;

            float3 direction = normalize(world_pos);
            float3 to_sun = normalize(sun_info.xyz);
            float sun_light_scatter = dot(direction, -to_sun);
            float3 res = float3(0, 0, 0);
            float l = 0;

            uint4 materials = (Material_tex.Load(IN.DispatchThreadID.xyz));
            if (materials.r == 0)
            {
                l = AtmosphereLength(direction, lighting_CB[FrameInfoCB.frame_index].m_CameraPosition.xyz);
  
                float sun = smoothstep(0.99996, 0.99997, -sun_light_scatter);
                res = sun * sun_info.w * lighting_CB[FrameInfoCB.frame_index].m_SunColor.xyz;

            }
            else
            {

                world_pos.xyz += lighting_CB[FrameInfoCB.frame_index].m_CameraPosition.xyz;

                materials -= 1;
                const uint material_offset = 2;

                //uint4 materials = NonUniformResourceIndex(Material_tex.Load(IN.DispatchThreadID.xyz));
                float4 UV = UV_tex.Load(int3(IN.DispatchThreadID.xy, 0));
                float4 Ddx_Ddy = Ddx_Ddy_tex.Load(IN.DispatchThreadID.xyz);



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
                float3 Normal_smpl = textures[materials.r * material_offset + 1].SampleGrad(sampl, UV.xy, Ddx_Ddy.xy, Ddx_Ddy.zw).xyz * 2.0 - 1.0;
                Normal_smpl.y = -Normal_smpl.y;
                float3 Normal = mul(TBN, float4(Normal_smpl, 0.0f)).xyz;
                //float3 Normal = mul(TBN, float4(0.0,0.0,1.0 ,0.0f)).xyz;



                float sun_light_diffuse = dot(Normal, normalize(sun_info.xyz));
                float sun_light = sun_info.w;
                float moon_light_diffuse = dot(Normal, normalize(moon_info.xyz));
                float moon_light = moon_info.w;

                sun_light *= 0.2 + clamp(sun_light_diffuse, 0.0, 1.0) * shadows.x;// *shadows.x;// *shadows.x;
                float sun_intens = smoothstep(-0.1, -0.0, sun_info.y);
                sun_light *= sun_intens * sun_intens * sun_intens;
                moon_light *= 0.2 + clamp(moon_light_diffuse, 0.0, 1.0);// *shadows.x;// *shadows.x;
                moon_light *= smoothstep(-0.3, -0.0, moon_info.y);// *shadows.x;// *shadows.x;

                float3 sun_light_color = sun_light * lighting_CB[FrameInfoCB.frame_index].m_SunColor.xyz;
                float3 moon_light_color = moon_light * lighting_CB[FrameInfoCB.frame_index].m_MoonColor.xyz;

                float3 Color = textures[materials.r * material_offset].SampleGrad(sampl, UV.xy, Ddx_Ddy.xy, Ddx_Ddy.zw).xyz;
                Color = pow(Color, 2.2);
                res = sun_light_color * Color + moon_light_color * Color;
            }
            float3 scattering = RayleighScatteringWavelength * RayleighScatteringMul;
            static const int numpoints = 30;
            static const int numsecondpoints = 10;

            float3 light = sun_info.w * lighting_CB[FrameInfoCB.frame_index].m_SunColor.xyz;
            float3 scattering_l_prev = 0; 
            float3 l_prev = l;
            //float tst_prev = EeartRadius * EeartRadius;
            float sqrEarthRadius = EarthRadius * EarthRadius;
            float tst_prev = sqrEarthRadius;

            for (int i = numpoints; i > 0; i--)
            {
                //float curr_l = l * FastInverse(i / numpoints);
                float curr_l = l * (i / numpoints);
                float3 curr_pos = world_pos.xyz + direction * curr_l;
                float distribution = RayleighDistribution(EarthHeight(curr_pos));
                float3 scattering_l = scattering * distribution;
                float d_l = l_prev - curr_l;

                res *= RayleighTransmittance(d_l * (scattering_l + scattering_l_prev) * 0.5);
                float tst = EarthTest(to_sun, curr_pos);
                float sun_ray_l = AtmosphereLength(to_sun, curr_pos);
                float distribution2 = distribution;
                for (int k = numsecondpoints; k > 0; k--)
                {
                    distribution2 += RayleighDistribution(EarthHeight(curr_pos + sun_ray_l * k/ numsecondpoints));
                }
                distribution2 /= numsecondpoints;
                float vis = 1;// step((tst + tst_prev) * 0.5 - sqrEarthRadius);
                float3 inlight = light * RayleighTransmittance(sun_ray_l * scattering * distribution2);
                res += d_l * RayleighScatteringPhase(sun_light_scatter) * scattering_l * vis * inlight;

                scattering_l_prev = scattering_l;
                l_prev = curr_l;
                tst_prev = tst;
            }
            
            OutImage[FrameInfoCB.frame_index][IN.DispatchThreadID.xy] = float4(res, 1.0);
            //Color *= 0.01;
            //Color = pow(Color, 2.2);
            
            //OutImage[FrameInfoCB.frame_index][IN.DispatchThreadID.xy] = float4(world_pos.xyz, 1.0);
            OutImage[FrameInfoCB.frame_index][IN.DispatchThreadID.xy] = float4(res, 1.0);
        }
    }
}