
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
    float4 WorldPosition    : WORLD_POSITION;
    float4 Position : SV_Position;

    nointerpolation uint id : InstanceID;
};

Texture2D<float4> tex_height : register(t0);
sampler sampl : register(s0);

VertexShaderOutput main(VertexPosColor IN, uint id : SV_InstanceID)
{
    const float map_scale = 0.001f;
    const float height_scale = 10.0f;
    VertexShaderOutput OUT;
    //IN.Position.x += 2.0f * id;
    float4 terrain = tex_height.SampleLevel(sampl, IN.Position.xz * map_scale + float2(0.5f,0.5f), 0);
    IN.Position.y += terrain.g * height_scale;
    OUT.Position = mul(ModelViewProjectionCB.MVP, float4(IN.Position, 1.0f));
    OUT.WorldPosition = float4(IN.Position, 1.0f);
    OUT.id = id;
    return OUT;
}