#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texMap;
uniform sampler2D blurMap;
uniform sampler2D DepthMap;


void main()
{
	vec4 Diffuse = texture(texMap, TexCoords.xy);
	vec4 Diffuse_blur = texture(blurMap, TexCoords.xy);

    vec2 texelSize = 1.0 / textureSize(texMap, 0);




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

	d_depth /= d_depth +0.0001;

	
	float blur = 1.0 - clamp(d_depth,0.0,1.0);
	//blur = min(blur1,blur);

	float edge_blur = 0.6 + 0.4*smoothstep(0.0,0.4, blur*blur);

	FragColor = vec4((edge_blur*Diffuse.xyz + (1.0 - edge_blur)*Diffuse_blur.xyz), 1.0);
	//FragColor = vec4(Diffuse.xyz, 1.0);
}
