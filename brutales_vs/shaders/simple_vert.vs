#version 330 core
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec2 v_Tex;

out vec2 Tex;
out vec3 v_Depth;


uniform mat4 camera;


void main()
{
 	
	vec4 tv_Position = camera * vec4(a_Position.xyz, 1.0);
	v_Depth.xy = 0.5 * (tv_Position.xy + vec2(1.0,1.0));

	gl_Position = tv_Position;

	tv_Position /= tv_Position.w;
	float z = tv_Position.z *0.5 + 0.5;
	v_Depth.z = z; 
	Tex.x = v_Tex.x;
	Tex.y = 1.0 - v_Tex.y;
}
