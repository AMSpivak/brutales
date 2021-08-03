#version 400 core

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
uniform mat4 ProjInv;
uniform mat4 ViewInv;

uniform vec3 LightDir;
uniform vec4 LightColor;
uniform vec3 viewPos;

vec3 PositionFromDepth(vec2 screen)
{
	float depth = texture(PositionMap, screen).x ;
    
    float z = depth * 2.0 - 1.0;

    vec4 clipSpacePosition = vec4(screen * 2.0 - 1.0, z, 1.0);
    vec4 viewSpacePosition = ProjInv * clipSpacePosition;
    viewSpacePosition /= viewSpacePosition.w;

    vec4 worldSpacePosition = ViewInv * viewSpacePosition;

    return worldSpacePosition.xyz;
}


float ShadowCalculation(vec4 PosLight, vec2 offset)
{
    
    vec4 fragPosLightSpace = lightSpaceMatrix * PosLight;
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    float depth = projCoords.z;
    float abs_x =abs(projCoords.x);
    float abs_y =abs(projCoords.y);
    float abs_z =depth;

    if(abs_x>1.0||abs_y>1.0)
        discard;
    if(abs_z>1.0)
        discard;
        //return 1.0;
    
    projCoords = projCoords * 0.5 + 0.5;
    depth = projCoords.z;

    float bias = 0.0005;
    //float occluder = texture(shadowMap,projCoords.xy).r;
    vec4 occluder = textureGather(shadowMap,projCoords.xy + offset);
    //vec2 min_v = min(occluder.xy,occluder.zw);
    //float min_f = bias + min(min_v.x,min_v.y);
    //vec2 max_v = max(occluder.xy,occluder.zw);
    //float max_f = bias + max(max_v.x,max_v.y);
    //float shdw = 1.0 - smoothstep(min_f,max_f,depth);
    vec4 receiver = vec4(depth);
    vec4 weight = vec4(0.25);
    //vec4 sh = smoothstep(vec4(-0.01), vec4(-0.005), occluder - receiver);
    vec4 sh = step(vec4(-bias), occluder - receiver);
    float shdw = dot(sh,weight);
    float shadowing   = shdw;
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
    float metallness = min(1.0,(texColor.a -0.06) * 1.064);
    vec4 normal_map = texture(NormalMap, TexCoords);
	vec3 texNormal= normal_map.xyz;
    float s = sign(normal_map.x);
    texNormal.x = abs(normal_map.x) - 1.01;
    texNormal.z = s * (1.0 - sqrt(dot(texNormal.xy,texNormal.xy)));
   // vec3 ibl_diff = textureLod(skybox, TexCoords,1);
   
	vec3 FragPos= PositionFromDepth(TexCoords);//texture(PositionMap, TexCoords).xyz;
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


        
        float shadow_res = (ShadowCalculation(vec4(FragPos.xyz,1.0),vec2(0.0,0.0)))
        ;//+ 0.15 *(ShadowCalculation(vec4(FragPos.xyz,1.0),vec2(0.0006,0.0006)))
        //+ 0.15 *(ShadowCalculation(vec4(FragPos.xyz,1.0),vec2(0.0006,-0.0006)))
        //+ 0.15 *(ShadowCalculation(vec4(FragPos.xyz,1.0),vec2(-0.0006,0.0006)))
        //+ 0.15 *(ShadowCalculation(vec4(FragPos.xyz,1.0),vec2(-0.0006,-0.0006)));
        shadow_res = min(max(0,norm_l),shadow_res);
        shadow_res = 0.4f + 0.6f * shadow_res;
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
        
        gAlbedoSpec =shadow_res * LightColor;//vec4(Diff_color,1.0);
        gAlbedoSpec =vec4(Diff_color,1.0);
        //gAlbedoSpec =vec4(roughness,roughness,roughness,1.0);
        //gAlbedoSpec =vec4(1,1,1,1.0);
        //gAlbedoSpec =vec4(0,0,0,1.0);
        //gNormal =vec4(ShadowLightColor * specular,1.0);
        //gNormal =vec4(shadow_res*specular* norm_l,1.0);
        //vec3 reflection =  vec3(0,0,0);//kS*textureLod(skybox, refl, 8 * roug_sqr).rgb;
        vec3 reflection =  shadow_res * kS * textureLod(skybox, refl,9 * roughness).rgb;
        //vec3 reflection =  kS*textureLod(skybox, refl,0).rgb;
        //reflection =  kS*textureLod(skybox, refl, 7).rgb;

        gNormal =vec4(shadow_res*reflection,1.0);
    }
    //gNormal =vec4(0.0);
}
