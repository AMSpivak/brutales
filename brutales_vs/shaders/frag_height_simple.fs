#version 330 core

in vec3 v_Position;



layout (location = 0) out vec4 gAlbedoSpec;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gPosition;

uniform sampler2D Albedo_0;
uniform sampler2D Normal_0;
uniform sampler2D Utility_0;

uniform sampler2D Albedo_1;
uniform sampler2D Normal_1;
uniform sampler2D Utility_1;

uniform sampler2D Albedo_2;
uniform sampler2D Normal_2;
uniform sampler2D Utility_2;

void main()
{

	vec4 texColor = vec4(0.9,0.5,0.2,1.0);

    vec3 utility = vec3(0.0,0.1,0.9);
    vec4 pos = vec4(v_Position,utility.x);
    gPosition = pos;

    
    gNormal = vec4(0.0,1.0,0.0, utility.y);

	gAlbedoSpec = vec4(0.0,1.0,0.0, 0.06 + utility.z*0.94);//texColor;
}