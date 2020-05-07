#version 330 core
in vec3 v_Position;
in vec3 ourColor;
in vec2 InvTexCoord;
in vec4 FragPosLightSpace;
out vec4 color;

uniform sampler2D ourTexture;
uniform sampler2D shadowMap;
uniform vec3 LightDir;
float ShadowCalculation(vec4 fragPosLightSpace)
{

	float kernel_gauss[25] = float[](
        1,  4,  6,  4, 1,
		4, 16, 24, 16, 4,
		6, 24, 36, 24, 6,
		4, 16, 24, 16, 4,
		1,  4,  6,  4, 1
    );
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    //float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
	
	float bias = max(0.001 * (1.0 - max(dot(ourColor,LightDir),0.0)), 0.005); 
    float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	float pcfDepth = 0;
	int i =0;
	
	for(int x = -2; x <= 2; ++x)
	{
		for(int y = -2; y <= 2; ++y)
		{
			pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
			shadow += kernel_gauss[i]*(currentDepth - bias > pcfDepth ? 1.0 : 0.0);   
			i++;
		}    
	}
	//pcfDepth /= 256.0;
	//shadow = currentDepth - bias > pcfDepth ? 1.0 : 0.0;   
	shadow /= 256.0;
    return shadow; 
}

void main()
{
	vec4 texColor = texture(ourTexture, InvTexCoord);
    if(texColor.a < 0.1)
        discard;

	float norm_l = max(dot(ourColor,LightDir),0.0);
	float shadow_res = 1.0-ShadowCalculation(FragPosLightSpace);
    color = LightDir.y*(0.3 +0.7*(shadow_res) *norm_l) * texColor;
}
