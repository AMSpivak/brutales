struct PixelShaderInput
{
    float4 Color    : COLOR;
    float4 Position : SV_Position;

    nointerpolation uint id: InstanceID;
};

Texture2D tex1[2] : register(t0);
sampler sampl : register(s0);

float4 main(PixelShaderInput IN) : SV_Target
{
    int material = IN.id;

//return tex1[material].Sample(sampl, IN.Color.xy);
return tex1[NonUniformResourceIndex(material)].Sample(sampl, IN.Color.xy);

//if (IN.id)
//{
//    return tex1[0].Sample(sampl, IN.Color.xy); //IN.Color;
//}
//else
//{
//    return tex1[1].Sample(sampl, IN.Color.xy); //IN.Color;
//}
}