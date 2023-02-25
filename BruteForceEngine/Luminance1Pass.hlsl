

struct PixelShaderInput
{
    float4 Position : SV_Position;
    float2 Tex : TexCoord0;
};

float Luminance(in float3 color)
{
    return log(0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b + 1.0);
}

Texture2D textureHDR : register(t0);
sampler sampl : register(s0);

float main(PixelShaderInput IN) : SV_TARGET
{
    float4 color = textureHDR.SampleLevel(sampl, IN.Tex, 0);
	return Luminance(color.xyz);
}