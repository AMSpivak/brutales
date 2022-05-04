
struct VertexShaderOutput
{
    float4 Position : SV_Position;
    float2 Tex : TexCoord0;
};

VertexShaderOutput main(in uint VertID : SV_VertexID)
{
    VertexShaderOutput OUT;
    OUT.Tex = float2(uint2(VertID, VertID << 1) & 2);
    OUT.Position = float4(lerp(float2(-1, 1), float2(1, -1), OUT.Tex), 0, 1);
    return OUT;
}