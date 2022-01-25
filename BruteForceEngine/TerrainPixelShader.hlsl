struct PixelShaderInput
{
    float4 Color    : COLOR;
    float4 Position : SV_Position;

    nointerpolation uint id: InstanceID;
};

Texture2D tex[] : register(t0);
sampler sampl[2] : register(s0);

float4 main(PixelShaderInput IN) : SV_Target
{
    int material = IN.id;

//return tex1[material].Sample(sampl, IN.Color.xy);
return tex[NonUniformResourceIndex(material)].Sample(sampl[1], IN.Color.xy);

//if (IN.id)
//{
//    return tex1[0].Sample(sampl, IN.Color.xy); //IN.Color;
//}
//else
//{
//    return tex1[1].Sample(sampl, IN.Color.xy); //IN.Color;
//}
}