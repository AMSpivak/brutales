
struct InvModelViewProjection
{
    matrix IMVP;
};

ConstantBuffer<InvModelViewProjection> InvModelViewProjectionCB : register(b0);
struct VertexShaderOutput
{
    float4 Position : SV_Position;
    float3 Normal : TexCoord0;
    float2 Tex : TexCoord1;
};

VertexShaderOutput main(in uint VertID : SV_VertexID)
{
    VertexShaderOutput OUT;
    OUT.Tex = float2(uint2(VertID << 1, VertID) & 2);
    OUT.Position = float4(lerp(float2(-1, 1), float2(1, -1), OUT.Tex), 0, 1);
    float4 normal = float4(OUT.Position.xy, 1.0f, 1.0f);
    normal = mul(InvModelViewProjectionCB.IMVP, normal);
    OUT.Normal = normalize(normal.xyz);
    return OUT;
}