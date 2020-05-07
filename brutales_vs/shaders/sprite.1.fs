#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texMap;


void main()
{
	vec3 col = vec3(0.0);
	vec3 vx = vec3(0.0);
	vec3 vy = vec3(0.0);
	vec3 cold = vec3(0.15 ,0.15,0.15);
	
	float kernel_x[9] = float[](
        -1, 0, 1,
        -2, 0, 2,
        -1, 0, 1
    );
	
	float kernel_y[9] = float[](
        -1, -2, -1,
         0,  0,  0,
         1,  2,  1
    );
	vec2 texelSize = 1.0 / textureSize(texMap, 0);
	int k = 0;
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			
			vx += kernel_x[k] * texture(texMap, TexCoords.xy + vec2(x, y) * texelSize).xyz; 
			vy += kernel_y[k] * texture(texMap, TexCoords.xy + vec2(x, y) * texelSize).xyz; 
			k++;
		}    
	}
	
	//float v = dot(vx,vx) + dot(vy,vy);
	//v = 0.25 *v;
	//v = pow(v,4);
	//FragColor = vec4(v,v,v, 1.0);
	vec4 texColor = texture(texMap, TexCoords);
    FragColor = vec4(texColor.xyz, 1.0);
}