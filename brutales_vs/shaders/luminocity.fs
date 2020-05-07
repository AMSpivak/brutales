#version 330 core
layout (location = 0) out vec4 gAlbedoSpec;
layout (location = 1) out vec4 gNormal;

in vec2 TexCoords;


uniform sampler2D PositionMap;


void main()
{

    vec4 texColor = texture(PositionMap, TexCoords);

	float v  = 0.1 + texColor.w;
    gNormal = vec4(0,0,0,0);
    gAlbedoSpec =vec4(v,v,v,1.0);//texColor;// LightDir.y*(0.3 +0.7*(shadow_res) *norm_l) * texColor;
}
