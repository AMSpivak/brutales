
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

float3 uncharted2_tonemap_partial(float3 x)
{
    float A = 0.15f;
    float B = 0.50f;
    float C = 0.10f;
    float D = 0.20f;
    float E = 0.02f;
    float F = 0.30f;
    return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
}

float3 uncharted2_filmic(float3 v, float exposure_bias)
{
    float3 curr = uncharted2_tonemap_partial(v * exposure_bias);

    float3 W = float3(11.2f,11.2f,11.2f);
    float3 white_scale = float3(1.0f, 1.0f, 1.0f) / uncharted2_tonemap_partial(W);
    return curr * white_scale;
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
    const float3 c_night = float3(0.077f, 0.73f, 3.3f);
    const float3 c_day = float3(1.f, 1.f, 1.f);
    float p = 1.0f;
    float3 color = lerp(c_night, c_day, p) * texture0.Sample(sampl, IN.Tex).xyz;

    float exposure_bias = 0.05f;
    return float4(uncharted2_filmic(color, exposure_bias), 1.0f);
}