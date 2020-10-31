#version 330 core
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec3 a_Tangent;
layout (location = 4) in ivec4 indexOfBone;
layout (location = 5) in vec4 a_BoneWeights;

out vec4 v_Position;



uniform mat4 model;
uniform mat4 draw;
uniform mat4 camera;
//uniform mat4 projection;
uniform mat4    u_BoneMatrices[128];

void main()
{
 	
	vec4 tv_Position = a_BoneWeights.x*(u_BoneMatrices[indexOfBone.x]* vec4(a_Position.xyz, 1.0));
	tv_Position = tv_Position + a_BoneWeights.y*(u_BoneMatrices[indexOfBone.y]* vec4(a_Position.xyz, 1.0));
	tv_Position = tv_Position +a_BoneWeights.z*(u_BoneMatrices[indexOfBone.z]* vec4(a_Position.xyz, 1.0));
	tv_Position = tv_Position +a_BoneWeights.w*(u_BoneMatrices[indexOfBone.w]* vec4(a_Position.xyz, 1.0));
	tv_Position = draw * model *(vec4(tv_Position.xyz, 1.0));
    v_Position = camera * vec4(tv_Position.xyz,1.0);



        gl_Position = v_Position;
}
