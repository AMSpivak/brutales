#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texMap;
uniform sampler2D LightMap;
uniform sampler2D SpecMap;
uniform sampler2D DepthMap;

void main()
{
	float d = texture(DepthMap, TexCoords.xy).x ;
	vec4 Diffuse= texture(texMap, TexCoords.xy);

	if(Diffuse.w < 0.05)
        discard;

    vec4 Light = texture(LightMap, TexCoords);
    vec4 Spec = texture(SpecMap, TexCoords);

 	//Light *= 0.6;
 	//Spec *= 0.4;
    vec2 texelSize = 1.0 / textureSize(LightMap, 0);





	Diffuse.w = 1.0;
	//Diffuse = vec4(1.0);
	//vec4 texColor = Diffuse*vec4(Light.xyz + vec3(0.03),1.0)+ vec4(Spec.xyz,0.0);
    vec4 texColor = Diffuse*vec4(Light.xyz,1.0)+ vec4(Spec.xyz,0.0);
	//vec4 texColor = vec4(Light.xyz,1.0)+ vec4(Spec.xyz,0.0);

	vec3 color = texColor.rgb / (texColor.rgb + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));  

	float edge_blur = 1.0f;//smoothstep(0.0,0.4, blur*blur);
	float intens = max(max(texColor.r, texColor.g),texColor.b) ;//length(texColor.xyz);

	float blur =  intens/(intens+0.1);

	float l=smoothstep(0.999,1.0, d);
	l*=0.25;

	vec3 atmosphere = vec3(0.9,0.9,1.0);

	FragColor = vec4((texColor.xyz*(1.0 - l)+l*atmosphere), 1.0);
	//FragColor = vec4(l,l,l, 1.0);
}
