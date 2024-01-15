
struct ViewProjection
{
    matrix VP;
};

struct Model
{
    matrix M;
};

struct Material
{
    uint Id;
};

ConstantBuffer<ViewProjection> ViewProjectionCB : register(b0);
ConstantBuffer<Model> ModelCB : register(b5);
ConstantBuffer<Material> MaterialCB : register(b10);

struct VertexPosColor
{
    float3 Position : POSITION;
    float2 Uv    : UV;
    float3 Normal    : NORMAL;
    float4 Tangent    : TANGENT;
    int4   BoneIndex    : B_INDEX;
    float4 BoneWeight    : B_WEIGHT;
};

struct VertexShaderOutput
{
    float4 Position : SV_Position;
    float2 Uv    : UV;
    float3 Normal    : NORMAL;
    float4 Tangent    : TANGENT;
    nointerpolation uint id : InstanceID;
};

VertexShaderOutput main(VertexPosColor IN, uint id : SV_InstanceID)
{
    VertexShaderOutput OUT;
    //IN.Position.x += 2.0f * id;
    float4 p = mul(ModelCB.M, float4(IN.Position, 1.0f));
    OUT.Position = mul(ViewProjectionCB.VP, p);
    OUT.Uv = IN.Uv;
    OUT.Normal = IN.Normal;
    OUT.Tangent = IN.Tangent;
    OUT.Normal = mul(ModelCB.M,float4(IN.Normal, 0.0f)).xyz;
    OUT.Tangent.w = IN.Tangent.w;
    OUT.Tangent.xyz = mul(ModelCB.M,float4(IN.Tangent.xyz, 0.0f)).xyz;
    OUT.id = MaterialCB.Id;
    return OUT;
}