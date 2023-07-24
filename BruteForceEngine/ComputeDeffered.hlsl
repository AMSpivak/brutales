#include "DefferedLightingCB.h"
#include "MaterialCB.h"
#include "CommonComputeInput.h"
#include "math.hlsli"
#include "LightScattering.hlsli"


struct FrameInfo
{
    uint frame_index;
};
ConstantBuffer<FrameInfo> FrameInfoCB : register(b0);

ConstantBuffer<DefferedLightingCB> lighting_CB[2] : register(b2);
ConstantBuffer<MaterialCB> Material_CB[2] : register(b5);

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

//float EarthShadow(float2 position) //world_pos.xz
//{
//    float2 l_dir = lighting_CB[FrameInfoCB.frame_index].m_SunShadow.xy;
//
//    float2 pos = position * lighting_CB[FrameInfoCB.frame_index].m_TerrainScaler.xz;
//    float2 shadowUV = pos + float2(l_dir.x + l_dir.y, l_dir.x - l_dir.y) * 0.5;
//    shadowUV = l_dir * shadowUV.x + float2(-l_dir.y, l_dir.x) * shadowUV.y;
//    shadowUV = float2(0.5f, 0.5f) + (shadowUV - float2(0.5f, 0.5f)) * lighting_CB[FrameInfoCB.frame_index].m_SunShadow.z;
//    if (shadowUV.x < 0 || shadowUV.x > 1 || shadowUV.y < 0 || shadowUV.y > 1)
//        return 1.0;
//    //float4 shadows = shadow_tex[FrameInfoCB.frame_index].SampleLevel(sampl, shadowUV, 0);
//    float4 shadows = shadow_tex[0].SampleLevel(sampl, shadowUV, 0);
//    return smoothstep(shadows.y, shadows.x, shadows.z);
//}
//
//float EarthShadow(float3 position) //world_pos.xz
//{
//    float2 l_dir = lighting_CB[FrameInfoCB.frame_index].m_SunShadow.xy;
//
//    float2 pos = position.xz * lighting_CB[FrameInfoCB.frame_index].m_TerrainScaler.xz;
//    float2 shadowUV = pos + float2(l_dir.x + l_dir.y, l_dir.x - l_dir.y) * 0.5;
//    shadowUV = l_dir * shadowUV.x + float2(-l_dir.y, l_dir.x) * shadowUV.y;
//    shadowUV = float2(0.5f, 0.5f) + (shadowUV - float2(0.5f, 0.5f)) * lighting_CB[FrameInfoCB.frame_index].m_SunShadow.z;
//    if (shadowUV.x < 0 || shadowUV.x > 1 || shadowUV.y < 0 || shadowUV.y > 1)
//        return 1.0;
//    //float4 shadows = shadow_tex[FrameInfoCB.frame_index].SampleLevel(sampl, shadowUV, 0);
//    float4 shadows = shadow_tex[0].SampleLevel(sampl, shadowUV, 0);
//    return smoothstep(shadows.y, shadows.x, position.y);
//}

//float2 EarthShadow(float3 position) //world_pos.xz
//{
//    float2 l_dir = lighting_CB[FrameInfoCB.frame_index].m_SunShadow.xy;
//
//    float2 pos = position.xz * lighting_CB[FrameInfoCB.frame_index].m_TerrainScaler.xz;
//    float2 shadowUV = pos + float2(l_dir.x + l_dir.y, l_dir.x - l_dir.y) * 0.5;
//    shadowUV = l_dir * shadowUV.x + float2(-l_dir.y, l_dir.x) * shadowUV.y;
//    shadowUV = float2(0.5f, 0.5f) + (shadowUV - float2(0.5f, 0.5f)) * lighting_CB[FrameInfoCB.frame_index].m_SunShadow.z;
//    if (shadowUV.x < 0 || shadowUV.x > 1 || shadowUV.y < 0 || shadowUV.y > 1)
//        return 1.0;
//    //float4 shadows = shadow_tex[FrameInfoCB.frame_index].SampleLevel(sampl, shadowUV, 0);
//    float4 shadows = shadow_tex[0].SampleLevel(sampl, shadowUV, 0);
//    return float2(smoothstep(shadows.y, shadows.x, position.y), smoothstep(shadows.w, shadows.z, position.y));
//}

float2 EarthShadow(float3 position) 
{

    uint FrameIndex = FrameInfoCB.frame_index && (uint(0xffff));
    float4 shadows = float4(0.0f, 0.0f, 0.0f, 0.0f);


    float2 pos = position.xz * lighting_CB[FrameIndex].m_TerrainScaler.xz;
    //if (pos.x > 0 && pos.x < 1 && pos.y > 0 && pos.y < 1)
    {
        float3 l_dir = lighting_CB[FrameIndex].m_SunShadow.xyz;
        float2 shadowUV = pos + float2(l_dir.x + l_dir.y, l_dir.x - l_dir.y) * 0.5;
        shadowUV = l_dir * shadowUV.x + float2(-l_dir.y, l_dir.x) * shadowUV.y;
        shadowUV = float2(0.5f, 0.5f) + (shadowUV - float2(0.5f, 0.5f)) * l_dir.z;
        if (shadowUV.x > 0 && shadowUV.x < 1 && shadowUV.y > 0 && shadowUV.y < 1)
        {
            shadows.xy = shadow_tex[0].SampleLevel(sampl, shadowUV, 0).xy;
        }

        l_dir = lighting_CB[FrameIndex].m_MoonShadow.xyz;
        shadowUV = pos + float2(l_dir.x + l_dir.y, l_dir.x - l_dir.y) * 0.5;
        shadowUV = l_dir * shadowUV.x + float2(-l_dir.y, l_dir.x) * shadowUV.y;
        shadowUV = float2(0.5f, 0.5f) + (shadowUV - float2(0.5f, 0.5f)) * l_dir.z;
        if (shadowUV.x > 0 && shadowUV.x < 1 && shadowUV.y > 0 && shadowUV.y < 1)
        {
            shadows.zw = shadow_tex[0].SampleLevel(sampl, shadowUV, 0).zw;;
        }
    }
    return float2(smoothstep(shadows.y, shadows.x, position.y), smoothstep(shadows.w, shadows.z, position.y));
}

#define BLOCK_SIZE 8
[numthreads(BLOCK_SIZE, BLOCK_SIZE, 1)]
void main(ComputeShaderInput IN)
{


    static const float3 ZeroEdge = float3(0.001, 0.001, 0.001);

    uint w = 0;
    uint h = 0;
    TBN_Quat_tex.GetDimensions(w, h);
    uint FrameIndex = FrameInfoCB.frame_index & (uint(0xffff));
    if(IN.DispatchThreadID.x < w && IN.DispatchThreadID.y < h)
    {
        //if (IN.DispatchThreadID.x & 1)
        {
            float4 sun_info = lighting_CB[FrameIndex].m_SunInfo;
            float4 moon_info = lighting_CB[FrameIndex].m_MoonInfo;

            

            float depth_val = depth.Load(IN.DispatchThreadID.xyz).r;// *2.0 - 1.0;
            float4 screen_pos = float4(IN.DispatchThreadID.xy, depth_val, 1);
            screen_pos.x = (screen_pos.x / w) * 2.0 - 1.0;
            screen_pos.y = (screen_pos.y / h) * 2.0 - 1.0;
            screen_pos.y *= -1.0;
            float4 world_pos = mul(lighting_CB[FrameIndex].m_CameraInverse, screen_pos);
            world_pos /= world_pos.w;

            float3 direction = normalize(world_pos.xyz);
            float3 to_sun = normalize(sun_info.xyz);
            float3 to_moon = normalize(moon_info.xyz);
            float sun_light_scatter = dot(direction, to_sun);
            float3 res = OutImage[FrameIndex][IN.DispatchThreadID.xy].xyz;
            //float3 res = float3(0, 0, 0);
            float l = length(world_pos.xyz);
            float sunscale = (smoothstep(0,abs(to_sun).y,0.1)*3 + 1.0) *0.00004;
            
            float sun = smoothstep(1.0 - sunscale, 1.0 - sunscale * 0.55, sun_light_scatter);
            sun_light_scatter = -lerp(sun_light_scatter, 1.0, sun);
            uint4 materials = NonUniformResourceIndex(Material_tex.Load(IN.DispatchThreadID.xyz));
            float3 camera = lighting_CB[FrameIndex].m_CameraPosition.xyz;
            world_pos.xyz += camera;

            float3 od_prev = float3(1, 1, 1);
            float3 ray_end = world_pos.xyz;
            if (materials.r == 0)
            {
                l = AtmosphereLength(direction, camera);
                float3 ray_end = camera + direction * (l);
                float l_shadow = SphereRay(to_sun, ray_end, EarthRadius, EarthCenter);
                float sun_ray_l = SphereRay(to_sun, ray_end, AtmosphereRadius, EarthCenter); //AtmosphereLength(to_sun, ray_end + to_sun * 100);
                float3 od = OpticalDepth(5, ray_end, to_sun, sun_ray_l - 10);
                od_prev = od;
                float3 sun_color = lighting_CB[FrameIndex].m_SunColor.xyz * sun_info.w * od;
                float vis = 1;
                res += max(ZeroEdge, sun * sun_color * vis);
                //res += sun;
            }
            else
            {             
                materials -= 1;
                const uint material_offset = 2;

                float4 UV = UV_tex.Load(int3(IN.DispatchThreadID.xy, 0));
                float4 Ddx_Ddy = Ddx_Ddy_tex.Load(IN.DispatchThreadID.xyz);

                //float shadow = EarthShadow(world_pos.xz);
                float2 shadow = EarthShadow(world_pos.xyz);
                
                float4 quat = (TBN_Quat_tex.Load(IN.DispatchThreadID.xyz));

                matrix TBN;
                mat_cast_xm(quat, TBN);
                //float3 Normal_smpl = normalize(float3(0.0, 0.3, 1.0));// textures[materials.r * material_offset + 1].SampleGrad(sampl, UV.xy, Ddx_Ddy.xy, Ddx_Ddy.zw).xyz * 2.0 - 1.0;
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

                float3 sun_color = lighting_CB[FrameIndex].m_SunColor.xyz * sun_info.w * od;

                float3 sun_light_color_direct = clamp(sun_light_diffuse, 0.0, 1.0) * shadow.x * sun_color;// smoothstep(-0.001, -0.0, sun_info.y);
                float3 sun_light_color_ambient = sun_color * (0.05f + 0.15 *clamp(to_sun.y+0.1, 0.0, 1.0));
                float3 sun_light_color = sun_light_color_ambient + sun_light_color_direct;


                sun_ray_l = AtmosphereLength(to_moon, world_pos.xyz);
                od = OpticalDepth(5, world_pos.xyz, to_moon, sun_ray_l);
                float3 moon_color = lighting_CB[FrameIndex].m_MoonColor.xyz * moon_info.w * od;

                float3 moon_light_color_direct = clamp(moon_light_diffuse, 0.0, 1.0) * shadow.y * moon_color;// smoothstep(-0.001, -0.0, sun_info.y);
                float3 moon_light_color_ambient = moon_color * (0.05f + 0.15 * clamp(to_moon.y + 0.1, 0.0, 1.0));
                float3 moon_light_color = moon_light_color_direct + moon_light_color_ambient;

                float3 Color = textures[materials.r * material_offset].SampleGrad(sampl, UV.xy, Ddx_Ddy.xy, Ddx_Ddy.zw).xyz;
                Color = pow(Color, 2.2);
                res = max(ZeroEdge, sun_light_color * Color + moon_light_color * Color);
            }

            static const int numpoints = 10;
            static const int numsecondpoints = 10;

            float3 light = sun_info.w * lighting_CB[FrameIndex].m_SunColor.xyz;
            float3 moon_source_light = moon_info.w * lighting_CB[FrameIndex].m_MoonColor.xyz;
            //float3 ray_end = camera + direction * l;
            float h_l = EarthHeight(ray_end);
            float l_prev = l;

            float sqrEarthRadius = EarthRadius * EarthRadius;
            float tst_prev = sqrEarthRadius;
            
            float3 scattering_l_prev = 0; 
            float3 transmittance_l_prev = 0;

            float3 fogsPhase[FOGS];
            for (int f = 0; f < FOGS; f++)
            {
                fogsPhase[f] = FogScatteringPhase(fogs[f], sun_light_scatter);
                float3 distribution = FogDistribution(fogs[f], h) ;
                transmittance_l_prev += distribution * (fogs[f].m_ScatteringParams0.xyz + fogs[f].m_ScatteringParams3.xyz);
                scattering_l_prev += distribution * fogsPhase[f] * fogs[f].m_ScatteringParams0.xyz;
            }

            float3 moon_light = sun_info.w * lighting_CB[FrameIndex].m_SunColor.xyz;


            float l_shadow =  SphereRay(to_sun, ray_end, EarthRadius, EarthCenter);// temp hack to check
            //for (int i = numpoints - 1; i > 0; i--)
            float l_near = l > 60 ? 30.0f : l / 2;
            float l_far = l - l_near;

            float3 scattering_l = 0;
            float3 transmittance_l = 0;

            for (int i = numpoints  - 1; i > 0; i--)
            {
                //float curr_l = l * FastInverse((float)i / numpoints);
                float curr_l = l_near + l_far * ((float)i) / numpoints;
                float3 curr_pos = camera + direction * curr_l;
                float h = EarthHeight(curr_pos);

                

                for (int f = 0; f < FOGS; f++)
                {
                    float3 distribution = FogDistribution(fogs[f], h);
                    transmittance_l += distribution * (fogs[f].m_ScatteringParams0.xyz + fogs[f].m_ScatteringParams3.xyz);
                    scattering_l += distribution * fogsPhase[f] * fogs[f].m_ScatteringParams0.xyz;
                }

                float d_l = l_prev - curr_l;

                res *= Transmittance(d_l * (transmittance_l + transmittance_l_prev) * 0.5);

                               
                float sun_ray_l = AtmosphereLength(to_sun, curr_pos);
                float moon_ray_l = AtmosphereLength(to_moon, curr_pos);
                l_shadow = SphereRay(to_sun, curr_pos, EarthRadius, EarthCenter);

                {
                    float2 shadow = EarthShadow(curr_pos);
                    float3 od = OpticalDepth(numsecondpoints, curr_pos, to_sun, sun_ray_l);
                    float3 inlight = light * od * shadow.x;
                    od = OpticalDepth(numsecondpoints, curr_pos, to_moon, moon_ray_l);
                    inlight += od * moon_source_light *shadow.y;
                    res += d_l * (scattering_l + scattering_l_prev) * 0.5 * inlight;
                }
                scattering_l_prev = scattering_l;
                transmittance_l_prev = transmittance_l;
                l_prev = curr_l;
            }

            for (int i = numpoints - 1; i > 0; i--)
            {
                //float curr_l = l * FastInverse((float)i / numpoints);
                float curr_l = l_near * ((float)i) / numpoints;
                float3 curr_pos = camera + direction * curr_l;
                float h = EarthHeight(curr_pos);

                float d_l = l_prev - curr_l;

                res *= Transmittance(d_l * (transmittance_l + transmittance_l_prev) * 0.5);


                float sun_ray_l = AtmosphereLength(to_sun, curr_pos);
                float moon_ray_l = AtmosphereLength(to_moon, curr_pos);
                l_shadow = SphereRay(to_sun, curr_pos, EarthRadius, EarthCenter);

                {
                    float2 shadow = EarthShadow(curr_pos);
                    float3 od = OpticalDepth(numsecondpoints, curr_pos, to_sun, sun_ray_l);
                    float3 inlight = light * od * shadow.x;
                    od = OpticalDepth(numsecondpoints, curr_pos, to_moon, moon_ray_l);
                    inlight += od * moon_source_light * shadow.y;
                    res += d_l * (scattering_l + scattering_l_prev) * 0.5 * inlight;
                }
                scattering_l_prev = scattering_l;
                transmittance_l_prev = transmittance_l;
                l_prev = curr_l;
            }

            
            OutImage[FrameIndex][IN.DispatchThreadID.xy] = float4(res, 1.0);
            //Color *= 0.01;
            //Color = pow(Color, 2.2);
            
            //OutImage[FrameInfoCB.frame_index][IN.DispatchThreadID.xy] = 0.001 * l_shadow4;// float4(0.001 * l_shadow, 0.001 * l, 0, 1.0);
            //OutImage[FrameInfoCB.frame_index][IN.DispatchThreadID.xy] = float4(0.001 * l_shadow, 0.001 *l,0.001 * (l - l_shadow), 1.0);
            //OutImage[FrameInfoCB.frame_index][IN.DispatchThreadID.xy] = (SphereOnRayShadow4(direction, lighting_CB[FrameInfoCB.frame_index].m_CameraPosition.xyz, to_sun, EarthRadius, EarthCenter));
        }
    }
}