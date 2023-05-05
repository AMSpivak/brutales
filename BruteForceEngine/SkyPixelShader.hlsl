struct FrameInfo
{
	uint frame_index;
};
ConstantBuffer<FrameInfo> FrameInfoCB : register(b0);

struct InvModelViewProjection
{
	matrix IMVP;
};

ConstantBuffer<InvModelViewProjection> InvModelViewProjectionCB : register(b1);

#include "SkyPixelCB.h"

ConstantBuffer<SkyPixelCB> SkyPixelsCB[3] : register(b17);
//Kosua20
float Hash(float n)
{
	return frac((1.0 + sin(n)) * 415.92653);
}
float Noise3d(float3 v)
{
	float xh = Hash(round(400 * v.x) * 37.0);
	float yh = Hash(round(400 * v.y) * 57.0);
	float zh = Hash(round(400 * v.z) * 67.0);
	return frac(xh + yh + zh);
}

struct PixelShaderInput
{
    float4 Position : SV_Position;
    float3 Normal : TexCoord0;
    float2 Tex : TexCoord1;
};

Texture2D SkyTextures[2] : register(t0);
SamplerState LinearClampSampler : register(s0);

float4 main(PixelShaderInput IN) : SV_TARGET
{
	float4 normal = float4(IN.Tex * 2 - 1.0 , 1.0f, 1.0f);
	normal.y = -normal.y;
	float4 normal_m = mul(InvModelViewProjectionCB.IMVP, normal);
	normal_m.xyz /= normal_m.w;
	float3 Normal = normalize(normal_m.xyz);

	float l = smoothstep(0.0,0.3, Normal.y);
	float l_earth = smoothstep(-0.05,0.0, Normal.y);

	float4 LightColor = SkyPixelsCB[FrameInfoCB.frame_index].LightColor;

	float3 sun = LightColor.xyz;//vec3(5.9,5.0,6.0);

	float3 sun_dir = normalize(SkyPixelsCB[FrameInfoCB.frame_index].LightDir.xyz);
	float3 EarthColor =  0.5 * float3(0.5,0.3,0.02) * max(sun_dir.y, 0.0);

	float to_sun = dot(Normal,sun_dir);

	float3 moon_dir = normalize(SkyPixelsCB[FrameInfoCB.frame_index].MoonDirection.xyz);
	float to_moon = dot(Normal, moon_dir);


	float sun_l = smoothstep(1.0 - 0.05 * (6.0f - 5.0f * l),1.0,to_sun) * 0.5 * (1.05 - smoothstep(0.05, 0.5, sun_dir.y));
	float a = smoothstep(0.05,0.9,to_sun);
	float a_earth = 0.2 + 0.8 * smoothstep(-0.1,0.9,sun_dir.y);

	float3 base = float3(0.9, 0.9, 1.0);

	float3 atmosphere = min(base, lerp(base, sun, a));
	float3 sky = (SkyPixelsCB[FrameInfoCB.frame_index].SkyColor.xyz * (1.0 - sun_l) + sun_l * sun);
	float t = (6.0f - 5.0f * l);
	sun_l = smoothstep(1.0 - 0.0005 * t, 1.0 - 0.0003 * t, to_sun);
	sky += sun_l * sun;

	t = 1.0;// (6.0f - 5.0f * l);
	sun_l = smoothstep(1.0 - 0.0005 * t, 1.0 - 0.0003 * t, to_moon);
	

	float atm = (1.0 - l)*0.5;
	float3 day = float3(atmosphere * (atm)+sky * (1.0 - atm));
	float star_intens = min(0, sun_dir.y);
	float star = smoothstep(0.99994,1.0,Noise3d(Normal));
	float3 night = star * star * float3(1.0,1.0,1.0) *0.1;
	//float night_intens = 1.0 - smoothstep(0.0,0.5,-star_intens + (1.0 - to_sun) * 0.08);
	//float3 sky_color = lerp(night,day,night_intens);

	float day_intencivity = smoothstep(-0.1, 0.0, sun_dir.y);
	float3 sky_color = night;// +day_intencivity * (day * LightColor.w);
	if (sun_l > 0)
	{
		float2 uv = float2(0.5, 0.5);
		float4 normal_x = mul(InvModelViewProjectionCB.IMVP, normal + float4(0.1f, 0 , 0, 0));
		normal_x.xyz /= normal_x.w;
		float4 normal_y = mul(InvModelViewProjectionCB.IMVP, normal + float4(0, 0.1f, 0, 0));
		normal_y.xyz /= normal_y.w;

		float3 v_x = normalize(normal_x.xyz - normal_m.xyz);
		float3 v_y = normalize(normal_y.xyz - normal_m.xyz);
		float3 d_uv = Normal - moon_dir;
		float scaler = (0.45f / 0.03f);
		float2 xy = float2(dot(d_uv, v_x),-dot(d_uv, v_y));
		xy *= scaler;
		uv += xy;
		float3 moon_normal = SkyTextures[1].Sample(LinearClampSampler, uv).xyz;

		float3 v_z = -Normal;
		v_x = cross(Normal, v_y);
		v_y = cross(v_z, v_x);
		//moon_normal = normalize(moon_normal.x * v_x + moon_normal.y * v_y + moon_normal.z * v_z);
		moon_normal = normalize(moon_normal.x  * v_x + moon_normal.y * v_y + moon_normal.z * v_z);
		sun_dir = float3(0, -1, 0);
		float lighted = max(dot(sun_dir, moon_normal), 0);
		sky_color += lighted * sun_l * float3(0.5, 0.5, 1.0) * SkyTextures[0].Sample(LinearClampSampler, uv);// SkyPixelsCB[FrameInfoCB.frame_index].MoonColor; //sky += sun_l * SkyPixelsCB[FrameInfoCB.frame_index].MoonColor;
	}
																																					
																																					//return float4(LightColor.w * lerp(EarthColor * a_earth, sky_color, l_earth), 1.0);
	return float4(sky_color, 1.0);
	//return float4(0,0,100, 1.0);
}