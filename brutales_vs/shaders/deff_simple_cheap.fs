#version 330 core
#define M_PI 3.1415926535897932384626433832795

in vec2 TexCoords;

layout (location = 0) out vec4 gAlbedoSpec;
layout (location = 1) out vec4 gNormal;

uniform sampler2D DiffuseMap;
uniform sampler2D NormalMap;
uniform sampler2D PositionMap;

uniform vec4 LightLocation;
uniform vec3 LightColor;
uniform vec3 viewPos;

float GGX_PartialGeometry(float cosThetaN, float alpha) {
    float cosTheta_sqr = clamp(cosThetaN*cosThetaN, 0.0, 1.0);
    float tan2 = ( 1 - cosTheta_sqr ) / cosTheta_sqr;
    float GP = 2 / ( 1 + sqrt( 1 + alpha * alpha * tan2 ) );
    return GP;
}

float GGX_Distribution(float cosThetaNH, float alpha) {
    float alpha2 = alpha * alpha;
    float NH_sqr = clamp(cosThetaNH*cosThetaNH, 0.0, 1.0);
    float den = NH_sqr * alpha2 + (1.0 - NH_sqr);
    return alpha2 / ( M_PI * den * den );
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (vec3(1.0) - F0) * pow(1.0 - cosTheta, 5.0);
}

void main()
{
    float rad = LightLocation.w;
	vec4 texColor = texture(DiffuseMap, TexCoords);
    
    if(texColor.a < 0.05)
        discard;

    vec3 FragPos= texture(PositionMap, TexCoords).xyz;
    vec3 LightVec = LightLocation.xyz - FragPos;

    float length_to_source = length(LightVec);
    
    if(length_to_source>rad)
        discard;


	vec4 normal_map = texture(NormalMap, TexCoords);

    vec3 viewDir = normalize(viewPos - FragPos);
    
    vec3 LightDir = normalize(LightVec);
	vec3 texNormal= normal_map.xyz;
    //texNormal = normalize(texNormal * 2.0 - 1.0); 
    float norm_l = max(dot(texNormal,LightDir),0);
    //float norm_l = abs(dot(texNormal,LightDir));

    vec3 halfwayDir = normalize(LightDir + viewDir); 
    float dotHV = max(dot(viewDir, halfwayDir), 0.0);
    float dotNH = max(dot(texNormal, halfwayDir), 0.0);
    float dotNV = max(dot(texNormal, viewDir), 0.1);

    //float f0 = mix(0.004,0.7,texColor.a);
    //float shlick =f0 + (1.0-f0)*pow((1.0 -norm_l),5);

    vec3 F0 = vec3(0.04);
    F0      = mix(F0, texColor.rgb, texColor.a);

    vec3 shlick =fresnelSchlick(dotNV,F0);
    float roug_sqr = normal_map.w*normal_map.w;
    float G = GGX_PartialGeometry(dotNV, roug_sqr) * GGX_PartialGeometry(norm_l, roug_sqr);
    float D = GGX_Distribution(dotNH, roug_sqr);    

    vec3 spec = G*shlick*D*0.25/(max(0.0, dotNV)+0.001);
    spec =max(vec3(0.0), spec);


    float intensivity = 1.0 - smoothstep(rad*0.3,rad,length_to_source);
    vec3 diffuse = clamp(vec3(1.0) - shlick, 0.0, 1.0);

    vec3 res = intensivity *(diffuse*norm_l/M_PI);
    gAlbedoSpec =vec4(((res) )* LightColor * vec3(1.0,1.0,1.0),1.0);
   // gAlbedoSpec =vec4(((norm_l) )* LightColor * vec3(1.0,1.0,1.0),1.0);
    gNormal =vec4(((intensivity*spec) )* LightColor * vec3(1.0,1.0,1.0),1.0);
}
