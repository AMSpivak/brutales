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

float EarthShadow(float2 position) //world_pos.xz
{
    float2 l_dir = lighting_CB[FrameInfoCB.frame_index].m_SunShadow.xy;

    float2 pos = position * lighting_CB[FrameInfoCB.frame_index].m_TerrainScaler.xz;
    float2 shadowUV = pos + float2(l_dir.x + l_dir.y, l_dir.x - l_dir.y) * 0.5;
    shadowUV = l_dir * shadowUV.x + float2(-l_dir.y, l_dir.x) * shadowUV.y;
    shadowUV = float2(0.5f, 0.5f) + (shadowUV - float2(0.5f, 0.5f)) * lighting_CB[FrameInfoCB.frame_index].m_SunShadow.z;

    float4 shadows = shadow_tex[FrameInfoCB.frame_index].SampleLevel(sampl, shadowUV, 0);
    return smoothstep(shadows.y, shadows.x, shadows.z);
}

float EarthShadow(float3 position) //world_pos.xz
{
    float2 l_dir = lighting_CB[FrameInfoCB.frame_index].m_SunShadow.xy;

    float2 pos = position.xz * lighting_CB[FrameInfoCB.frame_index].m_TerrainScaler.xz;
    float2 shadowUV = pos + float2(l_dir.x + l_dir.y, l_dir.x - l_dir.y) * 0.5;
    shadowUV = l_dir * shadowUV.x + float2(-l_dir.y, l_dir.x) * shadowUV.y;
    shadowUV = float2(0.5f, 0.5f) + (shadowUV - float2(0.5f, 0.5f)) * lighting_CB[FrameInfoCB.frame_index].m_SunShadow.z;

    float4 shadows = shadow_tex[FrameInfoCB.frame_index].SampleLevel(sampl, shadowUV, 0);
    return smoothstep(shadows.y, shadows.x, position.y);
}

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

            float3 direction = normalize(world_pos.xyz);
            float3 to_sun = normalize(sun_info.xyz);
            float sun_light_scatter = dot(direction, to_sun);
            float3 res = OutImage[FrameInfoCB.frame_index][IN.DispatchThreadID.xy].xyz;
            //float3 res = float3(0, 0, 0);
            float l = 0;
            float sunscale = (smoothstep(0,to_sun.y,0.1)*3 + 1.0) *0.00004;
            
            float sun = smoothstep(1.0 - sunscale, 1.0 - sunscale * 0.55, sun_light_scatter);
            sun_light_scatter = -lerp(sun_light_scatter, 1.0, sun);
            uint4 materials = /*NonUniformResourceIndex */ (Material_tex.Load(IN.DispatchThreadID.xyz));
            world_pos.xyz += lighting_CB[FrameInfoCB.frame_index].m_CameraPosition.xyz;
            if (materials.r == 0)
            {
                l = AtmosphereLength(direction, lighting_CB[FrameInfoCB.frame_index].m_CameraPosition.xyz);
                res += sun * sun_info.w * lighting_CB[FrameInfoCB.frame_index].m_SunColor.xyz;

            }
            else
            {             
                materials -= 1;
                const uint material_offset = 2;

                float4 UV = UV_tex.Load(int3(IN.DispatchThreadID.xy, 0));
                float4 Ddx_Ddy = Ddx_Ddy_tex.Load(IN.DispatchThreadID.xyz);



                /*float2 l_dir = lighting_CB[FrameInfoCB.frame_index].m_SunShadow.xy;

                float2 pos = world_pos.xz * lighting_CB[FrameInfoCB.frame_index].m_TerrainScaler.xz;
                float2 shadowUV = pos + float2(l_dir.x + l_dir.y, l_dir.x - l_dir.y) * 0.5;
                shadowUV = l_dir * shadowUV.x + float2(-l_dir.y, l_dir.x) * shadowUV.y;
                shadowUV = float2(0.5f, 0.5f) + (shadowUV - float2(0.5f, 0.5f)) * lighting_CB[FrameInfoCB.frame_index].m_SunShadow.z;

                float4 shadows = shadow_tex[FrameInfoCB.frame_index].SampleLevel(sampl, shadowUV, 0);
                shadows.x = smoothstep(shadows.y, shadows.x, shadows.z);*/
                float shadow = EarthShadow(world_pos.xz);
                
                matrix TBN;
                mat_cast_xm(quat, TBN);
                float3 Normal_smpl = textures[materials.r * material_offset + 1].SampleGrad(sampl, UV.xy, Ddx_Ddy.xy, Ddx_Ddy.zw).xyz * 2.0 - 1.0;
                Normal_smpl.y = -Normal_smpl.y;
                float3 Normal = mul(TBN, float4(Normal_smpl, 0.0f)).xyz;



                float sun_light_diffuse = dot(Normal, normalize(sun_info.xyz));
                float sun_light = sun_info.w;
                float moon_light_diffuse = dot(Normal, normalize(moon_info.xyz));
                float moon_light = moon_info.w;
                sun_light *= 0.2;
                
                float sun_intens = smoothstep(-0.1, -0.0, sun_info.y);
                sun_light *= sun_intens * sun_intens * sun_intens;
                moon_light *= 0.2 + clamp(moon_light_diffuse, 0.0, 1.0);
                moon_light *= smoothstep(-0.3, -0.0, moon_info.y);

                float sun_ray_l = AtmosphereLength(to_sun, world_pos.xyz);
                float3 od = OpticalDepth(5, world_pos.xyz, to_sun, sun_ray_l);

                float3 sun_color = lighting_CB[FrameInfoCB.frame_index].m_SunColor.xyz * sun_info.w * od;

                float3 sun_light_color_direct = clamp(sun_light_diffuse, 0.0, 1.0) * shadow * sun_color;// smoothstep(-0.001, -0.0, sun_info.y);
                float3 sun_light_color_ambient = sun_color * (0.05f + 0.15 *clamp(to_sun.y, 0.0, 1.0));
                float3 sun_light_color = sun_light_color_ambient + sun_light_color_direct;
                float3 moon_light_color = moon_light * lighting_CB[FrameInfoCB.frame_index].m_MoonColor.xyz;

                float3 Color = textures[materials.r * material_offset].SampleGrad(sampl, UV.xy, Ddx_Ddy.xy, Ddx_Ddy.zw).xyz;
                Color = pow(Color, 2.2);
                res = sun_light_color * Color + moon_light_color * Color;
            }

            float3 scattering = RayleighScatteringWavelength;
            static const int numpoints = 100;
            static const int numsecondpoints = 5;

            float3 light = sun_info.w * lighting_CB[FrameInfoCB.frame_index].m_SunColor.xyz;
            float h_l = EarthHeight(world_pos.xyz + direction * l);
            float l_prev = l;
            //float tst_prev = EeartRadius * EeartRadius;
            float sqrEarthRadius = EarthRadius * EarthRadius;
            float tst_prev = sqrEarthRadius;
            float RayleighPhase = RayleighScatteringPhase(sun_light_scatter);
            float MiePhase = MieScatteringPhase(sun_light_scatter, -0.55f);
            float3 scattering_l_prev = RayleighPhase * RayleighScatteringWavelength * RayleighDistribution(h_l) + MiePhase * MieScatteringWavelength * MieDistribution(h_l);


            float4 l_shadow4 = SphereOnRayShadow4(direction, lighting_CB[FrameInfoCB.frame_index].m_CameraPosition.xyz, to_sun, EarthRadius, EarthCenter);
            float l_shadow = l_shadow4.y;
            for (int i = numpoints - 1; i > 0; i--)
            {
                //float curr_l = l * FastInverse(i / numpoints);
                float curr_l = l * (i / numpoints);
                float3 curr_pos = world_pos.xyz + direction * curr_l;
                float h = EarthHeight(curr_pos);
                float distribution_r = RayleighDistribution(h);
                float distribution_m = MieDistribution(h);
                float3 scattering_l = RayleighPhase * RayleighScatteringWavelength * distribution_r + MiePhase * MieScatteringWavelength * distribution_m;

                float d_l = l_prev - curr_l;

                res *= Transmittance(d_l * (scattering_l + scattering_l_prev) * 0.5);
                //float tst = EarthTest(to_sun, curr_pos);


                if (l_shadow < l_prev)
                {
                    float sun_ray_l = AtmosphereLength(to_sun, curr_pos);

                    float3 od = OpticalDepth(numsecondpoints, curr_pos, to_sun, sun_ray_l);
                    float vis = step(l_shadow, curr_l);// saturate((curr_l - l_shadow) / (l_prev - curr_l));
                    //float vis = saturate((l_prev -l_shadow) / d_l); //step(l_shadow, l);
                    //float3 inlight = light * Transmittance(RayleighScatteringWavelength * distribution_r + MieScatteringWavelength * distribution_m);// +MieScatteringWavelength);
                    float3 inlight = light * od;// +MieScatteringWavelength);
                    float shadow = EarthShadow(curr_pos);
                    res += d_l * scattering_l * vis * inlight;
                }

                scattering_l_prev = scattering_l;
                l_prev = curr_l;
            }
            
            OutImage[FrameInfoCB.frame_index][IN.DispatchThreadID.xy] = float4(res, 1.0);
            //Color *= 0.01;
            //Color = pow(Color, 2.2);
            
            //OutImage[FrameInfoCB.frame_index][IN.DispatchThreadID.xy] = 0.001 * l_shadow4;// float4(0.001 * l_shadow, 0.001 * l, 0, 1.0);
            //OutImage[FrameInfoCB.frame_index][IN.DispatchThreadID.xy] = float4(0.001 * l_shadow, 0.001 *l,0.001 * (l - l_shadow), 1.0);
            //OutImage[FrameInfoCB.frame_index][IN.DispatchThreadID.xy] = (SphereOnRayShadow4(direction, lighting_CB[FrameInfoCB.frame_index].m_CameraPosition.xyz, to_sun, EarthRadius, EarthCenter));
        }
    }
}