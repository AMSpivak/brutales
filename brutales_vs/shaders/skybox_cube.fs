#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;




void main()
{
	
	vec4 Color = textureLod(skybox, TexCoords,0);
	FragColor = vec4(Color.xyz,1.0);



}