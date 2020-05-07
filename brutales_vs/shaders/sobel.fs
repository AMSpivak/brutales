#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texMap;
uniform sampler2D NormalMap;
uniform sampler2D DepthMap;

void main()
{
	vec3 col = vec3(0.0);
	vec3 vx = vec3(0.0);
	vec3 vy = vec3(0.0);
	vec3 vx_d = vec3(0.0);
	vec3 vy_d = vec3(0.0);
	vec3 cold = vec3(0.15 ,0.15,0.15);
	
	float kernel_x[9] = float[](
        3, 0, -3,
        10, 0, -10,
        3, 0, -3
    );
	
	float kernel_y[9] = float[](
        3, 10, 3,
         0,  0,  0,
         -3,  -10,  -3
    );
	vec2 texelSize = 1.0 / textureSize(NormalMap, 0);
	int k = 0;
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			
			vx += 0.5 * kernel_x[k] * texture(NormalMap, TexCoords.xy + vec2(x, y) * texelSize).xyz; 
			vy += 0.5 * kernel_y[k] * texture(NormalMap, TexCoords.xy + vec2(x, y) * texelSize).xyz;
			vx_d += kernel_x[k] * texture(DepthMap, TexCoords.xy + vec2(x, y) * texelSize).xyz; 
			vy_d += kernel_y[k] * texture(DepthMap, TexCoords.xy + vec2(x, y) * texelSize).xyz; 
			
			k++;
		}    
	}
	
	float v = (dot(vx,vx) + dot(vy,vy));
	v +=(dot(vx_d,vx_d) + dot(vy_d,vy_d));
	//v = 0.25 *v;
	//v = pow(v,4);
	//FragColor = (1.0 - v)*texture(texMap, TexCoords);
	//vec4(v,v,v, 1.0);
	vec4 texColor = texture(texMap, TexCoords);
    FragColor = vec4(texColor.xyz, 1.0);
}