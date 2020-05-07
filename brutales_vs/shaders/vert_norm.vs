#version 330 core
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec3 a_Tangent;

layout (location = 4) in ivec4 indexOfBone;
layout (location = 5) in vec4 a_BoneWeights;

out vec3 v_Position;
out vec3 ourColor;
//out vec3 ourColorView;
out vec2 TexCoord;
out mat3 TBN;


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
	tv_Position = draw*model *(vec4(tv_Position.xyz, 1.0));
        v_Position = tv_Position.xyz;




        ourColor = a_BoneWeights.x*vec3(draw*model *u_BoneMatrices[indexOfBone.x]* vec4(a_Normal, 0.0));
        ourColor = ourColor+a_BoneWeights.y*vec3(draw*model *u_BoneMatrices[indexOfBone.y]* vec4(a_Normal, 0.0));
        ourColor = ourColor+a_BoneWeights.z*vec3(draw*model *u_BoneMatrices[indexOfBone.z]* vec4(a_Normal, 0.0));
        ourColor = ourColor+a_BoneWeights.w*vec3(draw*model *u_BoneMatrices[indexOfBone.w]* vec4(a_Normal, 0.0));
		//ourColor = normalize((camera * vec4(ourColor,1.0)).xyz);
		//ourColorView = normalize((camera * vec4(ourColor,1.0)).xyz);

	
        vec3 tangent = a_BoneWeights.x*vec3(draw*model *u_BoneMatrices[indexOfBone.x]* vec4(a_Tangent, 0.0));
        tangent = tangent+a_BoneWeights.y*vec3(draw*model *u_BoneMatrices[indexOfBone.y]* vec4(a_Tangent, 0.0));
        tangent = tangent+a_BoneWeights.z*vec3(draw*model *u_BoneMatrices[indexOfBone.z]* vec4(a_Tangent, 0.0));
        tangent = tangent+a_BoneWeights.w*vec3(draw*model *u_BoneMatrices[indexOfBone.w]* vec4(a_Tangent, 0.0));
		//tangent = normalize((camera * vec4(tangent,1.0)).xyz);

		//ourColorView = normalize((camera * vec4(ourColor,1.0)).xyz);
		tangent = normalize(tangent - dot(tangent, ourColor) * ourColor);
// then retrieve perpendicular vector B with the cross product of T and N
		vec3 B = cross(ourColor, tangent);

		TBN = mat3(tangent, B, ourColor);
		
        gl_Position = camera * vec4(v_Position,1.0);


    	TexCoord.x = texCoord.x;
		TexCoord.y = 1.0 - texCoord.y;
}
