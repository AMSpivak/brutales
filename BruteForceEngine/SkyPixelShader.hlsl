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

float4 main(PixelShaderInput IN) : SV_TARGET
{
	float4 normal = float4(IN.Tex * 2 - 1.0 , 1.0f, 1.0f);
	normal.y = -normal.y;
	float4 normal_m = mul(InvModelViewProjectionCB.IMVP, normal);
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
	sun_l = smoothstep(1.0 - 0.0005 *t,1.0 - 0.0003 * t,to_sun);
	sky = sky * (1.0 - sun_l) + sun_l * sun;

	t = 1.0;// (6.0f - 5.0f * l);
	sun_l = smoothstep(1.0 - 0.0005 * t, 1.0 - 0.0003 * t, to_moon);
	

	float atm = (1.0 - l)*0.5;
	float3 day = float3(atmosphere * (atm)+sky * (1.0 - atm));
	float star_intens = min(0, sun_dir.y);
	float star = smoothstep(0.99994 + 0.008 * star_intens,1.0,Noise3d(Normal));
	float3 night = star * star * float3(1.0,1.0,1.0);
	float night_intens = 1.0 - smoothstep(0.0,0.5,-star_intens + (1.0 - to_sun) * 0.08);
	float3 sky_color = lerp(night,day,night_intens);
	sky_color += sun_l * SkyPixelsCB[FrameInfoCB.frame_index].MoonColor; //sky += sun_l * SkyPixelsCB[FrameInfoCB.frame_index].MoonColor;
	sky_color = night;
	return float4(LightColor.w * lerp(EarthColor * a_earth, sky_color, l_earth), 1.0);
	//return float4(LightColor.w * night, 1.0);
	//return float4(0,0,100, 1.0);
}