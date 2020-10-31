#version 330 core

#define M_PI 3.1415926535897932384626433832795


//out vec4 FragColor;
in vec2 TexCoords;

layout (location = 0) out vec4 gAlbedoSpec;
layout (location = 1) out vec4 gNormal;


uniform sampler2D DiffuseMap;
uniform sampler2D NormalMap;
uniform sampler2D PositionMap;
uniform sampler2D shadowMap;
uniform samplerCube skybox;

//uniform sampler2D shadowMap;
//uniform sampler2D shadowMap;

uniform mat4 lightSpaceMatrix;

uniform vec3 LightDir;
uniform vec4 LightColor;
uniform vec3 viewPos;


float ShadowCalculation(vec4 PosLight)
{
    
    vec4 fragPosLightSpace = lightSpaceMatrix * PosLight;
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    float depth = projCoords.z;
    float abs_x =abs(fragPosLightSpace.x);
    float abs_y =abs(fragPosLightSpace.y);

    if(abs_x>1.0||abs_y>1.0)
        return 1.0;


    projCoords = projCoords * 0.5 + 0.5;
    
    
    const float esm_bias   = 0.0;
    const float esm_factor = 10.0;
    float occluder = texture(shadowMap,projCoords.xy).r;
    float receiver = exp(esm_bias - esm_factor * depth);
    float shadowing   = clamp(occluder * receiver, 0.3, 1.0);
    return shadowing;

}


vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    //return F0 + (vec3(1.0,1.0,1.0) - F0) * pow(1.0 - cosTheta, 5.0);
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}


void main()
{
    // gAlbedoSpec =vec4(1.0,1.0,1.0,1.0);

    // gNormal =vec4(0.0,0.0,0.0,1.0);



	vec4 texColor = texture(DiffuseMap, TexCoords);
    if(texColor.a < 0.05)
        discard;
     //texColor.xyz = pow(texColor.xyz,vec3(2.2));
    float metallness = (texColor.a -0.06) * 1.064;
    vec4 normal_map = texture(NormalMap, TexCoords);
	vec3 texNormal= normal_map.xyz;

   // vec3 ibl_diff = textureLod(skybox, TexCoords,1);

	vec3 FragPos= texture(PositionMap, TexCoords).xyz;
	float norm_l = dot(texNormal,LightDir);
    
     
    //if(norm_l > 0.01)
    {
	    norm_l = max(norm_l,0.0);
        
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 refl = reflect(-viewDir,texNormal);
        vec3 halfwayDir = normalize(LightDir + viewDir); 
        float dotHV = max(dot(viewDir, halfwayDir), 0.0);
        float dotNH = max(dot(texNormal, halfwayDir), 0.0);
        float dotNV = max(dot(texNormal, viewDir), 0.10);

        vec3 F0 = vec3(0.04);
        F0 = mix(F0, normalize(texColor.rgb), metallness);

        vec3 shlick =fresnelSchlick(dotNV,F0);

        float roug_sqr = (normal_map.w)*(normal_map.w);
        float roughness = normal_map.w;
        roughness = max(roughness,0.1);
        vec3 shlick_rough =fresnelSchlickRoughness(dotNV,F0,roughness);


        //float D =  DistributionGGX(texNormal, halfwayDir, roughness);       
        //float G   = GeometrySmith(texNormal, viewDir, LightDir, roughness); 


        
        float shadow_res = (ShadowCalculation(vec4(FragPos.xyz,1.0)));
        //shadow_res = mix(1.0,shadow_res,LightColor.w );

        //vec3 numerator    = D * G * shlick;
        //float denominator = 4.0 * max(dotNV, 0.0);
        
        //float denominator = 4.0 * max(dotNV, 0.0) * norm_l + 0.001;
        //vec3 specular     = numerator / denominator;
        vec3 kS = shlick_rough;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallness;

        vec3 diffuse =kD/M_PI;
        //shadow_res = 1.0f;
        //kD = 1.0 - shlick_rough;
        //kD *= 1.0 - metallness;

        float mipper = 8;
        vec3 irradiance = textureLod(skybox, texNormal,mipper).rgb;
        //vec3 diffuse_c    = irradiance * kD* norm_l;
        vec3 diffuse_c    = irradiance * kD;
        //vec3 diffuse_c    = vec3(1)*norm_l;
        //shadow_res = 1.0f;
        vec3 Diff_color = (shadow_res) * diffuse_c;
        //vec3 Diff_color = diffuse_c;
        gAlbedoSpec =vec4(Diff_color,1.0);
        //gAlbedoSpec =vec4(roughness,roughness,roughness,1.0);
        //gAlbedoSpec =vec4(1,1,1,1.0);
        //gAlbedoSpec =vec4(0,0,0,1.0);
        //gNormal =vec4(ShadowLightColor * specular,1.0);
        //gNormal =vec4(shadow_res*specular* norm_l,1.0);
        //vec3 reflection =  vec3(0,0,0);//kS*textureLod(skybox, refl, 8 * roug_sqr).rgb;
        vec3 reflection =  shadow_res * kS*textureLod(skybox, refl,9 * roughness).rgb;
        //vec3 reflection =  kS*textureLod(skybox, refl,0).rgb;
        //reflection =  kS*textureLod(skybox, refl, 7).rgb;

        gNormal =vec4(shadow_res*reflection,1.0);
    }
    //gNormal =vec4(0.0);
}
