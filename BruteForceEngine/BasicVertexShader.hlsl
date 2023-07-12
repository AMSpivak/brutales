
struct ModelViewProjection
{
    matrix MVP;
};

ConstantBuffer<ModelViewProjection> ModelViewProjectionCB : register(b0);

struct VertexPosColor
{
    float3 Position : POSITION;
    float2 Uv    : UV;
    float3 Normal    : NORMAL;
    float3 Tangent    : TANGENT;
};

struct VertexShaderOutput
{
    float4 Position : SV_Position;
    float2 Uv    : UV;
    float3 Normal    : NORMAL;
    float3 Tangent    : TANGENT;
    nointerpolation uint id : InstanceID;
};

VertexShaderOutput main(VertexPosColor IN, uint id : SV_InstanceID)
{
    VertexShaderOutput OUT;
    IN.Position.x += 20.0f * id;
    IN.Position.y += 120.0f;
    OUT.Position = mul(ModelViewProjectionCB.MVP, float4(IN.Position, 1.0f));
    OUT.Uv = IN.Uv;
    OUT.Normal = IN.Normal;
    OUT.Tangent = IN.Tangent;
    OUT.id = id;
    return OUT;
}