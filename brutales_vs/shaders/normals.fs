#version 330 core


layout (location = 0) out vec4 gAlbedoSpec;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gPosition;

void main()
{



	vec4 texColor = vec4(0.9,0.5,0.2,1.0);//texture(ourTexture, TexCoord);
    //if(texColor.a < 0.1)
    //    discard;

    vec3 utility = vec3(1.0,0.9,0.1);//texture(UtilityTexture, TexCoord).xyz;
    //vec4 pos = vec4(v_Position,utility.x);
    vec4 pos = vec4(1.0,0.9,0.1,utility.x);
    gPosition = pos;
    gNormal = vec4(vec3(0.0,1.0,0.0), utility.y);
	gAlbedoSpec = vec4(1.0,1.0,0.0, 0.06 + utility.z*0.94);//texColor;
}