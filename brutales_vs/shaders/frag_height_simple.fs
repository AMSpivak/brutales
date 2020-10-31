#version 330 core

in vec3 v_Position;



layout (location = 0) out vec4 ExpShadow;

uniform sampler2D ExpShadow;

void main()
{
    const float esm_factor = 0.5;
    ExpShadow = exp(esm_factor * v_Position.z);
	gAlbedoSpec = vec4(0.0,1.0,0.0, 0.06 + utility.z*0.94);//texColor;
}