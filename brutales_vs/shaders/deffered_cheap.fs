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

uniform mat4 lightSpaceMatrix;

uniform vec3 LightDir;
uniform vec3 LightColor;
uniform vec3 viewPos;

float ShadowCalculation(vec4 PosLight, vec3 tNormal)
{
	vec4 fragPosLightSpace = lightSpaceMatrix * PosLight;

    if(fragPosLightSpace.x<-fragPosLightSpace.w||fragPosLightSpace.x>fragPosLightSpace.w||fragPosLightSpace.y<-fragPosLightSpace.w||fragPosLightSpace.y>fragPosLightSpace.w)
        discard;

	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;



	projCoords = projCoords * 0.5 + 0.5;
    float currentDepth = projCoords.z;

    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	//float bias = max(0.01 * smoothstep(0.65,0.85,(1.0 - max(dot(tNormal,LightDir),0.0))), 0.005);

    vec2 shift = texelSize*vec2(0.0,1.0);
    //float res = (textureProj(shadowMap,vec4(projCoords,1.0)));
    float res = texture(shadowMap,vec3(projCoords.xy,currentDepth));

    return res;
}



vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (vec3(1.0) - F0) * pow(1.0 - cosTheta, 5.0);
}




void main()
{
	vec4 texColor = texture(DiffuseMap, TexCoords);
    //if(texColor.a < 0.05)
    //    discard;
    vec4 normal_map = texture(NormalMap, TexCoords);
	vec3 texNormal= normal_map.xyz;

	vec3 FragPos= texture(PositionMap, TexCoords).xyz;
	float norm_l = max(dot(texNormal,LightDir),0.01);

    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 halfwayDir = normalize(LightDir + viewDir); 
    float dotHV = max(dot(viewDir, halfwayDir), 0.0);


    vec3 F0 = vec3(0.04);
    F0      = mix(F0, texColor.rgb, texColor.a);

 

	float shadow_res =(ShadowCalculation(vec4(FragPos.xyz,1.0),texNormal));
    float shadow_norm =1.0 -  smoothstep(0.6,1.0,1.0-norm_l);//norm_l;//smoothstep(shadow_res,1.0,norm_l);
    shadow_res = 1.0 - (1.0 - shadow_res) * norm_l;

    float spec = max(dotHV, 0.0);
    
    vec3 specular     = F0*spec*spec;

    vec3 ShadowLightColor = shadow_res* LightColor*0.3;
    //vec3 ShadowLightColor = LightColor;
    gAlbedoSpec =vec4(ShadowLightColor *norm_l,1.0);
    //gAlbedoSpec =vec4(0.0);
    gNormal =vec4(ShadowLightColor * specular,1.0);
    //gNormal =vec4(0.0);
}
