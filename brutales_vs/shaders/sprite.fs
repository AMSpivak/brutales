#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texMap;


void main()
{
	
	
	//float v = dot(vx,vx) + dot(vy,vy);
	//v = 0.25 *v;
	//v = pow(v,4);
	//FragColor = vec4(v,v,v, 1.0);
	vec4 texColor = texture(texMap, TexCoords);
    FragColor = texColor;
	//vec4(texColor.xyz, 1.0);
}