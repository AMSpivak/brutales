#version 330 core

#define M_PI 3.1415926535897932384626433832795


//out vec4 FragColor;
in vec2 TexCoords;

layout (location = 0) out vec4 gAlbedoSpec;
layout (location = 1) out vec4 gNormal;


uniform sampler2D DiffuseMap;
uniform sampler2D NormalMap;
uniform sampler2D PositionMap;
uniform sampler2DShadow shadowMap;
//uniform sampler2D shadowMap;
//uniform sampler2D shadowMap;

uniform mat4 lightSpaceMatrix;

uniform vec3 LightDir;
uniform vec4 LightColor;
uniform vec3 viewPos;

// float ShadowCalculation(vec4 PosLight, vec3 tNormal)
// {
    
//     vec4 fragPosLightSpace = lightSpaceMatrix * PosLight;
//     vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

//     float abs_x =abs(fragPosLightSpace.x);
//     float abs_y =abs(fragPosLightSpace.y);

//     if(abs_x>1.0||abs_y>1.0)
//         return 1.0;

//     abs_x = smoothstep(0.85,1.0,abs_x);
//     abs_y = smoothstep(0.85,1.0,abs_y);
//     abs_x *= abs_y;
//         //discard;

//     float light_fall = dot(tNormal,LightDir);

//     projCoords = projCoords * 0.5 + 0.5;
//     float currentDepth = projCoords.z;

//     vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
//     float sqr_ang = (1.0 - max(light_fall,0.0));
//     //sqr_ang =sqr_ang ;//* sqr_ang* sqr_ang;
//     float bias = 0.02 * sqr_ang;

//     currentDepth -= bias;
//     vec2 shift = texelSize*vec2(0.0,1.0);
    

//     float Factor = 0.0;

//     float weight[5] = float[5](0.25,0.5,1.0,0.5,0.25);

//     float w_summ = 0.0;

//     for (int y = -2 ; y <= 2 ; y++) {
//         for (int x = -2 ; x <= 2 ; x++) {
//             vec2 Offsets = texelSize*vec2(x, y);
//             vec2 UVC = vec2(projCoords.xy + Offsets);
//             float res = smoothstep(-bias,0.0,texture(shadowMap, UVC).x - currentDepth);
//             float k = weight[2 + x]*weight[2 + y];
//             Factor += k* res;
//             w_summ +=k;
//         }
//     }
//     float swadowing =Factor/w_summ;
    
//     swadowing = smoothstep(0.0,light_fall, swadowing);
//     return swadowing;//mix(swadowing*swadowing,1.0,abs_x);
// }

float ShadowCalculation(vec4 PosLight, vec3 tNormal)
{
    
    vec4 fragPosLightSpace = lightSpaceMatrix * PosLight;
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    float abs_x =abs(fragPosLightSpace.x);
    float abs_y =abs(fragPosLightSpace.y);

    if(abs_x>1.0||abs_y>1.0)
        return 1.0;

    abs_x = smoothstep(0.85,1.0,abs_x);
    abs_y = smoothstep(0.85,1.0,abs_y);
    abs_x *= abs_y;
        //discard;

    float light_fall = dot(tNormal,LightDir);

    projCoords = projCoords * 0.5 + 0.5;
    float currentDepth = projCoords.z;

    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    float sqr_ang = (1.0 - max(light_fall,0.0));
    //sqr_ang =sqr_ang ;//* sqr_ang* sqr_ang;
    float bias = 0.00001 * sqr_ang;

    currentDepth += bias;
    //vec3 uvd = vec3(projCoords.xy,currentDepth);
    //float swadowing = texture(shadowMap, uvd);

    float Factor = 0.0;

    for (int y = -1 ; y <= 1 ; y++) {
        for (int x = -1 ; x <= 1 ; x++) {
            vec2 Offsets = vec2(x, y) * texelSize;
            vec3 UVC = vec3(projCoords.xy + Offsets, currentDepth);
            Factor += texture(shadowMap, UVC);
        }
    }

    //float shadowing = (0.5 + (Factor / 18.0));
    float shadowing = (Factor / 9.0);


    return shadowing;

}


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
    //return F0 + (vec3(1.0,1.0,1.0) - F0) * pow(1.0 - cosTheta, 5.0);
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = M_PI * denom * denom;
	
    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}


void main()
{
    // gAlbedoSpec =vec4(1.0,1.0,1.0,1.0);

    // gNormal =vec4(0.0,0.0,0.0,1.0);



	vec4 texColor = texture(DiffuseMap, TexCoords);
    if(texColor.a < 0.05)
        discard;

    float metallness = (texColor.a -0.06) * 1.064;
    vec4 normal_map = texture(NormalMap, TexCoords);
	vec3 texNormal= normal_map.xyz;

	vec3 FragPos= texture(PositionMap, TexCoords).xyz;
	float norm_l = dot(texNormal,LightDir);
    
     
    if(norm_l > 0.01)
    {
	    norm_l = max(norm_l,0.0);
        
        vec3 viewDir = normalize(viewPos - FragPos);

        vec3 halfwayDir = normalize(LightDir + viewDir); 
        float dotHV = max(dot(viewDir, halfwayDir), 0.0);
        float dotNH = max(dot(texNormal, halfwayDir), 0.0);
        float dotNV = max(dot(texNormal, viewDir), 0.10);

        vec3 F0 = vec3(0.04);
        F0 = mix(F0, texColor.rgb, metallness);

        vec3 shlick =fresnelSchlick(dotNV,F0);

        float roug_sqr = (normal_map.w)*(normal_map.w);
        float roughness = normal_map.w;
        roughness = max(roughness,0.1);

        float D =  DistributionGGX(texNormal, halfwayDir, roughness);       
        float G   = GeometrySmith(texNormal, viewDir, LightDir, roughness); 



        float shadow_res = (ShadowCalculation(vec4(FragPos.xyz,1.0),texNormal));
        shadow_res = mix(1.0,shadow_res,LightColor.w );

        vec3 numerator    = D * G * shlick;
        //float denominator = 4.0 * max(dotNV, 0.0);
        
        float denominator = 4.0 * max(dotNV, 0.0) * norm_l + 0.001;
        vec3 specular     = numerator / denominator;
        vec3 kS = shlick;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallness;

        vec3 diffuse =kD/M_PI;
        vec3 ShadowLightColor =  shadow_res* LightColor.xyz;
        gAlbedoSpec =vec4(ShadowLightColor *norm_l* diffuse,1.0);
        //gNormal =vec4(ShadowLightColor * specular,1.0);
        gNormal =vec4(shadow_res*specular* norm_l,1.0);

        //gNormal =vec4(0.0,0.0,0.0,1.0);
    }
    else
    {
        gAlbedoSpec =vec4(0.0,0.0,0.0,1.0);
        gNormal =vec4(0.0,0.0,0.0,1.0);
    }
    //gNormal =vec4(0.0);
}
