#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texMap;
uniform sampler2D NormalMap;
uniform sampler2D DepthMap;
uniform sampler2D LightMap;
uniform sampler2D SpecMap;

void main()
{
	vec4 Diffuse= texture(texMap, TexCoords.xy);

	if(Diffuse.w < 0.05)
        discard;

    vec4 Light = texture(LightMap, TexCoords);
    vec4 Spec = texture(SpecMap, TexCoords);


	vec2 texelSize = 0.7 / textureSize(NormalMap, 0);

	Diffuse.w = 1.0;
	vec3 color = Diffuse*Light.xyz+ Spec.xyz;
	color = color / (color + vec3(1.0));
	color = pow(color, vec3(1.0/2.2));

	vec4 texColor = vec4(color,1.0);

	//vec2 offset = TexCoords+ vec2(-1, -1)* texelSize;
	//Light = texture(LightMap, offset);
    //Spec = texture(SpecMap, offset);
	//vec4 texColor1 = texture(texMap, offset)*vec4(Light.xyz,1.0)+ vec4(Spec.xyz,0.0);

	//offset = TexCoords+ vec2(1, -1)* texelSize;
	//Light = texture(LightMap, offset);
    //Spec = texture(SpecMap, offset);
	//vec4 texColor2 = texture(texMap, offset)*vec4(Light.xyz,1.0)+ vec4(Spec.xyz,0.0);

	//offset = TexCoords+ vec2(-1, 1)* texelSize;
	//Light = texture(LightMap, offset);
    //Spec = texture(SpecMap, offset);
	//vec4 texColor3 = texture(texMap, offset)*vec4(Light.xyz,1.0)+ vec4(Spec.xyz,0.0);

	//offset = TexCoords+ vec2(1, 1)* texelSize;
	//Light = texture(LightMap, offset);
    //Spec = texture(SpecMap, offset);


	 
	//vec4 texColor4 = texture(texMap, offset)*vec4(Light.xyz,1.0)+ vec4(Spec.xyz,0.0);
	vec4 BlurColor = texColor;//(texColor1 + texColor2 + texColor3 + texColor4 + texColor)*0.2;



	vec3 vt = (texture(NormalMap, TexCoords.xy + vec2(-1, -1)* texelSize).xyz
					-texture(NormalMap, TexCoords.xy + vec2(1, 1)* texelSize ).xyz
					);
	vec3 vt2 = (texture(NormalMap, TexCoords.xy + vec2(-1, 1)* texelSize).xyz
					-texture(NormalMap, TexCoords.xy + vec2(1, -1)* texelSize ).xyz
					);
	vt = 0.5 *(vt + vt2);

	float v_n =dot(vt,vt2);



	float d_depth = abs(
						texture(DepthMap, TexCoords.xy + vec2(-1, -1) * texelSize).x -
						texture(DepthMap, TexCoords.xy + vec2(1, 1) * texelSize).x
					);
	d_depth += abs(
						texture(DepthMap, TexCoords.xy + vec2(-1, 1) * texelSize).x -
						texture(DepthMap, TexCoords.xy + vec2(1, -1) * texelSize).x
					);


	v_n /=v_n + 0.1;

	d_depth /= d_depth +0.01;

	float blur = (1.0-v_n )*(1.0 - d_depth);
	blur = clamp(blur,0.0,1.0);

    FragColor = vec4(texColor.xyz, 1.0);
}
