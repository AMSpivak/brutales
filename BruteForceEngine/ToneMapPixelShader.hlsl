
float luminance(float3 v)
{
    return dot(v, float3(0.2126f, 0.7152f, 0.0722f));
}

float3 change_luminance(float3 c_in, float l_out)
{
    float l_in = luminance(c_in);
    return c_in * (l_out / l_in);
}

float3 reinhard_extended_luminance(float3 v, float max_white_l)
{
    //float l_old = luminance(v);
    //float numerator = l_old * (1.0f + (l_old / (max_white_l * max_white_l)));
    //float l_new = numerator / (1.0f + l_old);
    //return change_luminance(v, l_new);
    return change_luminance(v, 1.0f);
}

struct PixelShaderInput
{
    float4 Position : SV_Position;
    float2 Tex : TexCoord0;
};

Texture2D texture0 : register(t0);
sampler sampl : register(s0);

float4 main(PixelShaderInput IN) : SV_Target
{
    float max_white = luminance(float3(100.0f,100.0f,100.0f));
    float3 color = texture0.Sample(sampl, IN.Tex).xyz;
    return float4(reinhard_extended_luminance(color, max_white), 1.0f);
}