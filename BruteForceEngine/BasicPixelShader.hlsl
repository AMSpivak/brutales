#include "math.hlsli"

struct PixelShaderInput
{
    float4 Position : SV_Position;
    float2 Uv    : UV;
    float3 Normal    : NORMAL;
    float4 Tangent    : TANGENT;
    nointerpolation uint id : InstanceID;
};


struct PS_OUTPUT
{
    float4 Normal: SV_Target0;
    uint4 Material: SV_Target1;
    float4 TexUV: SV_Target2;
    float4 TexDdxDdy: SV_Target3;
};
PS_OUTPUT main(PixelShaderInput IN) : SV_Target
{
    int material = IN.id;
    
    //return tex1[material].Sample(sampl, IN.Color.xy);
    //return tex1[NonUniformResourceIndex(material)].Sample(sampl, IN.Color.xy);
    float3 face_Normal = normalize(IN.Normal);
    //float3 B_Normal = normalize(cross(IN.Tangent, IN.Normal));
    float3 T_Normal = normalize(IN.Tangent.xyz);
    float3 B_Normal = normalize(cross(IN.Normal, IN.Tangent.xyz) *IN.Tangent.w);
    //float3 T_Normal = normalize(cross(B_Normal, IN.Normal));

    matrix TBN2 = transpose(matrix(float4(T_Normal, 0), float4(B_Normal, 0), float4(face_Normal, 0), float4(0, 0, 0, 1)));
    //matrix TBN2 = (matrix(float4(B_Normal, 0), float4(T_Normal, 0), float4(face_Normal, 0), float4(0, 0, 0, 1)));
    float4 quat_xm;
    quat_cast_xm(TBN2, quat_xm);
    float2 derivX = ddx(IN.Uv.xy);
    float2 derivY = ddy(IN.Uv.xy);

    PS_OUTPUT output;

    output.Normal = quat_xm;//Set second output
    output.Material = material + 1;
    output.TexDdxDdy = float4(derivX, derivY);
    output.TexUV = float4(IN.Uv.xy, 0.0, 0.0);
    return output;
}