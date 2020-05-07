#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texMap;
uniform sampler2D blurMap;
uniform sampler2D DepthMap;


void main()
{
	vec4 Diffuse = texture(texMap, TexCoords.xy);
	
	//FragColor = vec4((edge_blur*Diffuse.xyz + (1.0 - edge_blur)*Diffuse_blur.xyz), 1.0);
	FragColor = vec4(Diffuse.xyz, 1.0);
}
