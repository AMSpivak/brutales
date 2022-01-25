
struct ModelViewProjection
{
    matrix MVP;
};

ConstantBuffer<ModelViewProjection> ModelViewProjectionCB : register(b0);

struct VertexPosColor
{
    float3 Position : POSITION;
    float3 Color    : COLOR;
};

struct VertexShaderOutput
{
    float4 Color    : COLOR;
    float4 Position : SV_Position;

    nointerpolation uint id : InstanceID;
};

Texture2D<float4> tex[3] : register(t0);
sampler sampl[2] : register(s0);

VertexShaderOutput main(VertexPosColor IN, uint id : SV_InstanceID)
{
    VertexShaderOutput OUT;
    IN.Position.x += 2.0f * id;
    float4 terrain = tex[0].SampleLevel(sampl[0], IN.Position.xz, 0);
    IN.Position.y += terrain.g * 1.0f;
    OUT.Position = mul(ModelViewProjectionCB.MVP, float4(IN.Position, 1.0f));
    OUT.Color = float4(IN.Color, 1.0f);
    OUT.id = id;
    return OUT;
}