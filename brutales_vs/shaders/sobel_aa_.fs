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

 	Light *= 0.6;
 	Spec *= 0.4;
    vec2 texelSize = 1.0 / textureSize(LightMap, 0);

	// vec2 shift = texelSize * vec2(0.0, 1.0);
    // Light += 0.1 * (texture(LightMap,TexCoords + shift));
    // Spec  += 0.15 * (texture(SpecMap,TexCoords + shift));
    // shift =texelSize * vec2(0.0,-1.0);
    // Light += 0.1 * (texture(LightMap,TexCoords + shift));
    // Spec  += 0.15 * (texture(SpecMap,TexCoords + shift));
    // shift =texelSize * vec2(1.0, 0.0);
    // Light += 0.1 * (texture(LightMap,TexCoords + shift));
    // Spec  += 0.15 * (texture(SpecMap,TexCoords + shift));
    // shift = texelSize * vec2(-1.0, 0.0);
    // Light += 0.1 * (texture(LightMap,TexCoords + shift));
    // Spec  += 0.15 * (texture(SpecMap,TexCoords + shift));

	texelSize = 1.0 / textureSize(NormalMap, 0);

	Diffuse.w = 1.0;
	vec4 texColor = Diffuse*vec4(Light.xyz,1.0)+ vec4(Spec.xyz,0.0);

	

	//Light = texture(LightMap, TexCoords);
    //Spec = texture(SpecMap, TexCoords);


	vec3 vt = (texture(NormalMap, TexCoords.xy + vec2(-1, -1)* texelSize).xyz
					-texture(NormalMap, TexCoords.xy + vec2(1, 1)* texelSize ).xyz
					);
	vec3 vt2 = (texture(NormalMap, TexCoords.xy + vec2(-1, 1)* texelSize).xyz
					-texture(NormalMap, TexCoords.xy + vec2(1, -1)* texelSize ).xyz
					);
	vt = 0.5 *(vt + vt2);

	float v_n =dot(vt,vt2);

	float d1 = texture(DepthMap, TexCoords.xy + vec2(-1, -1) * texelSize).x ;
	float d2 = texture(DepthMap, TexCoords.xy + vec2(1, 1) * texelSize).x ;
	float d3 = texture(DepthMap, TexCoords.xy + vec2(-1, 1) * texelSize).x ;
	float d4 = texture(DepthMap, TexCoords.xy + vec2(1, -1) * texelSize).x ;
	float d = texture(DepthMap, TexCoords.xy).x ;
	float dif1 = (d1 - d);
	float dif2 = (d - d2);
	float dif3 = (d3 - d);
	float dif4 = (d - d4);

	float dif12 = 0.5*(dif1+dif2);
	float dif34 = 0.5*(dif3+dif4);

	float d_depth = abs(dif1 - dif12)+abs(dif2 - dif12)+abs(dif3 - dif34)+abs(dif4 - dif34);
	//d_depth*=d;
	//d_depth += abs((dif3 + dif4));
	


	//v_n /=v_n + 0.8;

	d_depth /= d_depth +0.0001;

	float blur1 =1.0-clamp(v_n,0.0,1.0);//*(1.0 - d_depth);
	blur1*=blur1*blur1;
	
	float blur = 1.0 - clamp(d_depth,0.0,1.0);
	blur = min(blur1,blur);

	//blur =smoothstep(0.0,0.2, blur* blur);
	// if(blur< 0.5)
	// {
	// //blur =smoothstep(0.0,0.2, blur* blur);
	// 	vec2 shift = texelSize * vec2(0.0, 1.0);
	// 	Light += 0.1 * (texture(LightMap,TexCoords + shift));
	// 	Spec  += 0.15 * (texture(SpecMap,TexCoords + shift));
	// 	shift =texelSize * vec2(0.0,-1.0);
	// 	Light += 0.1 * (texture(LightMap,TexCoords + shift));
	// 	Spec  += 0.15 * (texture(SpecMap,TexCoords + shift));
	// 	shift =texelSize * vec2(1.0, 0.0);
	// 	Light += 0.1 * (texture(LightMap,TexCoords + shift));
	// 	Spec  += 0.15 * (texture(SpecMap,TexCoords + shift));
	// 	shift = texelSize * vec2(-1.0, 0.0);
	// 	Light += 0.1 * (texture(LightMap,TexCoords + shift));
	// 	Spec  += 0.15 * (texture(SpecMap,TexCoords + shift));

	// 	vec4 texColor = Diffuse*vec4(Light.xyz,1.0)+ vec4(Spec.xyz,0.0);
	// 	blur =smoothstep(0.0,0.2, blur* blur);


	// }
	//blur =1.0f;//smoothstep(0.0,0.4, blur*blur);
	float edge_blur = 1.0f;//smoothstep(0.0,0.4, blur*blur);
	float intens = length(texColor.xyz);

	blur =  edge_blur * intens/(intens+0.01);
	float l=smoothstep(0.999,1.0, d);
	l*=0.25;
	// l*=0.0;
	vec3 atmosphere = vec3(0.9,0.9,1.0);

	FragColor = vec4((blur)*(texColor.xyz*(1.0 - l)+l*atmosphere), 1.0);
	//FragColor = vec4(l,l,l, 1.0);
}
