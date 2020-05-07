#version 330 core
in vec3 ourColor;
//in vec3 ourColorView;
in vec3 v_materials;
in vec3 v_Position;
in vec2 TexCoord;
//in vec3 Normal;
in mat3 TBN;

layout (location = 0) out vec4 gAlbedoSpec;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gPosition;
uniform sampler2D HeightMap; 

uniform sampler2D Albedo_0;
uniform sampler2D Normal_0;
uniform sampler2D Utility_0;

uniform sampler2D Albedo_1;
uniform sampler2D Normal_1;
uniform sampler2D Utility_1;

uniform sampler2D Albedo_2;
uniform sampler2D Normal_2;
uniform sampler2D Utility_2;

// float blend(float4 texture1, float a1, float4 texture2, float a2)
// {
//     float depth = 0.2;
//     float ma = max(texture1.a + a1, texture2.a + a2) - depth;

//     float b1 = max(texture1.a + a1 - ma, 0);
//     float b2 = max(texture2.a + a2 - ma, 0);

//     return (texture1.rgb * b1 + texture2.rgb * b2) / (b1 + b2);
// }

float blend( float a1,  float a2)
{
    float depth = 0.02;
    float ma = max(a1, a2) - depth;

    float b1 = max(a1 - ma, 0);
    float b2 = max(a2 - ma, 0);

    return (b1) / (b1 + b2);
}

void main()
{
    float tex_mul = 80.0;
    //vec4 notUsed = texture2D(HeightMap, vec2(0.0, 0.0));

    vec2 Coord = TexCoord*tex_mul;
	//vec4 texColor = vec4(0.9,0.5,0.2,1.0);//texture(AlbedoTexture, TexCoord);
	vec4 texColor = texture(Albedo_0, Coord);
	vec4 texColor1 = texture(Albedo_1, Coord);

    vec4 tmp = texture(Normal_0, Coord);
    vec3 normal = tmp.xyz;
    float h1 =0.5 *  tmp.w;

    tmp = texture(Normal_1, Coord);
    vec3 normal1 = tmp.xyz;
    float h2 =0.5 * tmp.w;

    float mix_value = blend(v_materials.x + h1 ,v_materials.y + h2);
    //texColor = v_materials.x*texColor + v_materials.y*texColor1;
    vec3 materials = vec3(mix_value,1.0 - mix_value,0.0);
    
    texColor = materials.x*texColor + materials.y*texColor1;

    //if(texColor.a < 0.1)
    //    discard;

    //vec3 utility = texture(UtilityTexture, TexCoord*tex_mul).xyz;//texture(UtilityTexture, TexCoord).xyz;
    //utility.x = 0.0;
    //utility.z = 0.0;
    //vec3 utility = vec3(0.0,0.9,0.1);
    float r_intensivity = 0.6;
    vec3 utility = vec3(0.0,1.0 - r_intensivity + texture(Utility_0, Coord).y * r_intensivity,0.1);
    vec3 utility1 = vec3(0.0,1.0 - r_intensivity + texture(Utility_1, Coord).y * r_intensivity,0.1);
    //vec3 utility = vec3(0.0,1.0 - texture(UtilityTexture, TexCoord*tex_mul).y *0.5,0.1);
    utility = utility*materials.x + utility1*materials.y;


    vec4 pos = vec4(v_Position,utility.x);
    gPosition = pos;

    //vec3 normal = vec3(0.0,1.0,0.0);

    
    normal = normal*materials.x + normal1*materials.y;
    //normal.y = -normal.y;
    normal = normalize(normal * 2.0 - 1.0); 
    normal = normalize(TBN * normal); 
    //normal = normal * 0.5 + 0.5; 
    
    gNormal = vec4(normal.xyz, utility.y);
    //gNormal = vec4(vec3(0.0,1.0,0.0), utility.y);

	gAlbedoSpec = vec4(texColor.xyz, 0.06 + utility.z*0.94);//texColor;
	//gAlbedoSpec = vec4(normal.xyz, 0.06 + utility.z*0.94);//texColor;
	//gAlbedoSpec = vec4(ourColor.xyz, 0.06 + utility.z*0.94);//texColor;

	
	//gAlbedoSpec = gNormal;
}