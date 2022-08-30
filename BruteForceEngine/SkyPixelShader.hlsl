
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

float3 LightDir;
float3 LightColor;
float3 SkyColor;

struct PixelShaderInput
{
    float4 Position : SV_Position;
    float3 Normal : TexCoord0;
    float2 Tex : TexCoord1;
};

float4 main(PixelShaderInput IN) : SV_TARGET
{
	float l = smoothstep(0.0,0.3, IN.Normal.y);
	float l_earth = smoothstep(-0.05,0.0, IN.Normal.y);

	float3 sun = 3 * LightColor;//vec3(5.9,5.0,6.0);


	float3 sun_dir = normalize(LightDir);
	float3 EarthColor = 0.5 * float3(0.5,0.3,0.02) * max(sun_dir.y, 0.0);

	float to_sun = dot(normalize(IN.Normal.xyz),sun_dir);

	float sun_l = smoothstep(1.0 - 0.02 * (6.0f - 5.0f * l),1.0,to_sun) * 0.05;
	float a = smoothstep(0.05,0.9,to_sun);
	float a_earth = 0.2 + 0.8 * smoothstep(0.0,0.9,sun_dir.y);

	float3 atmosphere = min(float3(0.9,0.9,1.0), lerp(float3(0.9,0.9,1.0),sun,a));
	//vec3 sky = vec3(1.0,1.0,1.1) *(1.0 -sun_l) + sun_l*sun;

	//vec3 sky = vec3(0.0,0.6,1.0) *(1.0 -sun_l) + sun_l*sun;
	float3 sky = SkyColor * (1.0 - sun_l) + sun_l * sun;
	sun_l = smoothstep(1.0 - 0.0005 * (6.0f - 5.0f * l),1.0,to_sun) * 0.9;
	sky = sky * (1.0 - sun_l) + sun_l * sun;
	float atm = (1.0 - l);
	float3 day = float3(atmosphere * (atm)+sky * (1.0 - atm));
	float star_intens = min(0, sun_dir.y);
	float star = smoothstep(0.99994 + 0.008 * star_intens,1.0,Noise3d(normalize(IN.Normal.xyz)));
	float3 night = star * star * float3(1.0,1.0,1.0);
	//float night_intens = smoothstep(-0.1,0.1, to_sun);
	float night_intens = 1.0 - smoothstep(0.0,0.5,-star_intens + (1.0 - to_sun) * 0.08);
	float3 sky_color = lerp(night,day,night_intens);
	
	return float4(lerp(EarthColor * a_earth, sky_color, l_earth), 1.0);
}