

struct PixelShaderInput
{
    float4 Position : SV_Position;
    float2 Tex : TexCoord0;
};

Texture2D texture0 : register(t0);
sampler sampl : register(s0);

float4 main(PixelShaderInput IN) : SV_Target
{
	return float4(texture0.Sample(sampl, IN.Tex).xyz, 1.0f);
}