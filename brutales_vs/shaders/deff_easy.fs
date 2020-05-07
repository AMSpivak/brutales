#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D DiffuseMap;
uniform sampler2D NormalMap;
uniform sampler2D PositionMap;
uniform sampler2D shadowMap;

uniform mat4 lightSpaceMatrix;

uniform vec3 LightDir;

float ShadowCalculation(vec4 PosLight, vec3 tNormal)
{
	vec4 fragPosLightSpace = lightSpaceMatrix * PosLight;

	float kernel_gauss[25] = float[](
        1,  4,  6,  4, 1,
		4, 16, 24, 16, 4,
		6, 24, 36, 24, 6,
		4, 16, 24, 16, 4,
		1,  4,  6,  4, 1
    );
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords = projCoords * 0.5 + 0.5;
    float currentDepth = projCoords.z;


	float bias = 0.001;// max(0.06 * (1.0 - max(dot(tNormal,LightDir),0.0)), 0.01);
    float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	float pcfDepth = 0;
	int i =0;


	int x = 0;
    int y = 0;
	for(int x = -2; x <= 2; ++x)
	{
		for(int y = -2; y <= 2; ++y)
		{
			pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += kernel_gauss[i]*(currentDepth - bias > pcfDepth ? 1.0 : 0.0);
			i++;
		}
	}


    //pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
    //shadow = 0;//(currentDepth - bias > pcfDepth ? 1.0 : 0.0);

	shadow /= 256.0;
    return shadow;
}

void main()
{
	vec4 texColor = texture(DiffuseMap, TexCoords);
    if(texColor.a < 0.1)
        discard;
	vec3 texNormal= texture(NormalMap, TexCoords).xyz;
	float norm_l = max(dot(texNormal,LightDir),0.0);
	float shadow_res = 1.0 - ShadowCalculation(vec4(texture(PositionMap, TexCoords).xyz,1.0),texNormal);
    FragColor =(0.3 +0.7 * (shadow_res) * norm_l) * texColor;// LightDir.y*(0.3 +0.7*(shadow_res) *norm_l) * texColor;
}
