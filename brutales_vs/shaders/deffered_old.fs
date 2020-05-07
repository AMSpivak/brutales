#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D DiffuseMap;
uniform sampler2D NormalMap;
uniform sampler2D PositionMap;

uniform vec3 LightDir;

void main()
{
	vec4 texColor = texture(DiffuseMap, TexCoords);
    if(texColor.a < 0.1)
        discard;
	vec3 texNormal= texture(NormalMap, TexCoords).xyz;
	float norm_l = max(dot(texNormal,LightDir),0.0);
	float shadow_res = 1.0;//-ShadowCalculation(FragPosLightSpace);
    FragColor = (0.3 +0.7*(shadow_res) *norm_l) * texColor;// LightDir.y*(0.3 +0.7*(shadow_res) *norm_l) * texColor;
}