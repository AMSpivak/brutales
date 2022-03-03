struct PixelShaderInput
{
    float4 WorldPosition  : WORLD_POSITION;
    float4 Position : SV_Position;

    nointerpolation uint id: InstanceID;
};

Texture2D<uint4> tex_material_id : register(t1);
Texture2D tex[] : register(t2);
sampler sampl[2] : register(s0);

float4 main(PixelShaderInput IN) : SV_Target
{
    //int material = IN.id;
    const float map_scale = 0.001f;
//return tex1[material].Sample(sampl, IN.Color.xy);
//return tex[NonUniformResourceIndex(material)].Sample(sampl[1], IN.Color.xy);
    int3 load_pos = int3(0, 0, 0);
    tex_material_id.GetDimensions(load_pos.x, load_pos.y);
    load_pos.xy = round((IN.WorldPosition.xz * map_scale + float2(0.5f, 0.5f)) * load_pos.xy);
    uint4 materials = NonUniformResourceIndex(tex_material_id.Load(load_pos));
    //uint4 materials = tex_material_id.Sample(sampl[2], IN.WorldPosition.xz * map_scale);
    //int3 load_pos = int3(0, 0, 0);
    //materials = clamp(materials, uint4(0, 0, 0, 0), uint4(1, 1, 1, 1));
    //uint4 materials = tex_material_id.Sample(sampl[1], (IN.WorldPosition.xz * map_scale + float2(0.5f,0.5f)));
    //return tex[materials.r].Sample(sampl[0], IN.WorldPosition.xz);
    return tex[materials.r].Sample(sampl[0], IN.WorldPosition.xz);

//if (IN.id)
//{
//    return tex1[0].Sample(sampl, IN.Color.xy); //IN.Color;
//}
//else
//{
//    return tex1[1].Sample(sampl, IN.Color.xy); //IN.Color;
//}
}