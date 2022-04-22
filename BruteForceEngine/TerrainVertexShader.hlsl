
struct ModelViewProjection
{
    matrix MVP;
};

ConstantBuffer<ModelViewProjection> ModelViewProjectionCB : register(b0);

struct PlanesBuffer
{
    float4 m_PlanesPositions[1024];
    float4 m_TerrainScaler;
};

ConstantBuffer<PlanesBuffer> PlanesCB : register(b17);

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
    float2 pos = (IN.Position.xz) * PlanesCB.m_PlanesPositions[id].z + PlanesCB.m_PlanesPositions[id].xy;
    IN.Position.x = pos.x;
    IN.Position.z = pos.y;
    float4 terrain = tex_height.SampleLevel(sampl, pos * map_scale + float2(0.5f,0.5f), 0);
    IN.Position.y += height_scale * terrain.g;
    OUT.Position = mul(ModelViewProjectionCB.MVP, float4(IN.Position, 1.0f));
    OUT.WorldPosition = float4(IN.Position, 1.0f);
    OUT.id = id;
    return OUT;
}