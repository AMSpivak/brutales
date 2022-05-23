
struct FrameInfo
{
    uint frame_index;
};
ConstantBuffer<FrameInfo> FrameInfoCB : register(b0);

struct ModelViewProjection
{
    matrix MVP;
};

ConstantBuffer<ModelViewProjection> ModelViewProjectionCB : register(b2);

struct PlanesBuffer
{
    float4 m_PlanesPositions[1024];
    float4 m_TerrainScaler;
};

ConstantBuffer<PlanesBuffer> PlanesCB[3] : register(b17);

struct VertexPosColor
{
    float3 Position : POSITION;
};

struct VertexShaderOutput
{
    float4 WorldPosition    : WORLD_POSITION;
    float4 Position : SV_Position;
    float3 Normal : NORMAL;

    nointerpolation uint id : InstanceID;
};

Texture2D<float4> tex_height : register(t0);
sampler sampl : register(s0);

VertexShaderOutput main(VertexPosColor IN, uint id : SV_InstanceID)
{
    VertexShaderOutput OUT;
    //IN.Position.x += 2.0f * id;
    float2 pos_p = (IN.Position.xz) * PlanesCB[FrameInfoCB.frame_index].m_PlanesPositions[id].z + PlanesCB[FrameInfoCB.frame_index].m_PlanesPositions[id].xy;
    IN.Position.x = pos_p.x;
    IN.Position.z = pos_p.y;
    uint t_width;
    uint t_height;
    
    tex_height.GetDimensions(t_width, t_height);
    float2 offset = float2(1.0f/ t_width, 1.0f/ t_height);
    //float2 offset = PlanesCB.m_TerrainScaler.xz * PlanesCB.m_PlanesPositions[id].z * PlanesCB.m_TerrainScaler.w;

    float2 pos = pos_p * PlanesCB[FrameInfoCB.frame_index].m_TerrainScaler.xz + float2(0.5f, 0.5f);
    float4 terrain = tex_height.SampleLevel(sampl, pos, 0);

    float3 terrain_x = PlanesCB[FrameInfoCB.frame_index].m_TerrainScaler.y * tex_height.SampleLevel(sampl, pos + float2(offset.x, 0.f), 0).xyz;
    terrain_x -= PlanesCB[FrameInfoCB.frame_index].m_TerrainScaler.y * tex_height.SampleLevel(sampl, pos + float2(-offset.x, 0.f), 0).xyz;
    terrain_x.y = terrain_x.r;
    terrain_x.z = 0.f;
    terrain_x.x = 2.0f * offset.x / PlanesCB[FrameInfoCB.frame_index].m_TerrainScaler.x;

    float3 terrain_z = PlanesCB[FrameInfoCB.frame_index].m_TerrainScaler.y * tex_height.SampleLevel(sampl, pos + float2(0.f, offset.y), 0).xyz;
    terrain_z -= PlanesCB[FrameInfoCB.frame_index].m_TerrainScaler.y * tex_height.SampleLevel(sampl, pos + float2(0.f, -offset.y), 0).xyz;
    terrain_z.y = terrain_z.r;
    terrain_z.x = 0.f;
    terrain_z.z = 2.0f * offset.y / PlanesCB[FrameInfoCB.frame_index].m_TerrainScaler.z;
    IN.Position.y += PlanesCB[FrameInfoCB.frame_index].m_TerrainScaler.y * terrain.r;
    OUT.Position = mul(ModelViewProjectionCB.MVP, float4(IN.Position, 1.0f));
    OUT.WorldPosition = float4(IN.Position, 1.0f);
    OUT.Normal = normalize(cross(terrain_z, terrain_x));
    OUT.id = id;
    return OUT;
}