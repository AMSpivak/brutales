#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

uniform vec3 LightDir;
uniform vec3 LightColor;

//Kosua20
float Hash(float n)
{
	return fract((1.0 +sin(n))*415.92653);
}
float Noise3d(vec3 v)
{
	float xh = Hash(round(400 * v.x)*37.0);
	float yh = Hash(round(400 * v.y)*57.0);
	float zh = Hash(round(400 * v.z)*67.0);
	return fract(xh + yh + zh);
}

void main()
{
	
	vec4 Color = texture(skybox, TexCoords);
	float l=smoothstep(0.0,0.3, TexCoords.y);

	vec3 sun = vec3(5.9,5.0,6.0);

	vec3 sun_dir = normalize(LightDir);
	float to_sun = dot(normalize(TexCoords.xyz),sun_dir);
	float sun_l = smoothstep(0.95,1.0,to_sun) *0.1;

	vec3 atmosphere = vec3(0.9,0.9,1.0);
	vec3 sky = vec3(0.0,0.6,1.0) *(1.0 -sun_l) + sun_l*sun;
	sun_l = smoothstep(0.9995,1.0,to_sun) *0.8;
	sky = sky *(1.0 -sun_l) + sun_l*sun;

	vec3 day =  vec3(atmosphere*(1.0-l)+sky*l);
	float star_intens = min(0, sun_dir.y);
	float star = smoothstep(0.99994 + 0.008 * star_intens,1.0,Noise3d(normalize(TexCoords.xyz)));
	vec3 night = star * star*vec3(1.0,1.0,1.0);
	float night_intens = smoothstep(0.0,0.5,-star_intens + (1.0 - to_sun) *0.08 );
	FragColor = vec4(mix(day,night,night_intens),1.0);


}