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
	vec3 EarthColor = vec3(0.5,0.3,0.02);
	float l=smoothstep(0.0,0.3, TexCoords.y);
	float l_earth=smoothstep(-0.05,0.0, TexCoords.y);

	vec3 sun = LightColor;//vec3(5.9,5.0,6.0);

	vec3 sun_dir = normalize(LightDir);
	float to_sun = dot(normalize(TexCoords.xyz),sun_dir);
	float sun_l = smoothstep(1.0 - 0.05 * (6.0f -  5.0f * l),1.0,to_sun) * 0.1;
	float a = smoothstep(0.05,0.9,to_sun);
	float a_earth = 0.2 + 0.8 *smoothstep(0.0,0.9,sun_dir.y);

	vec3 atmosphere = min(vec3(0.9,0.9,1.0),mix(vec3(0.9,0.9,1.0),sun,a));
	vec3 sky = vec3(0.0,0.6,1.0) *(1.0 -sun_l) + sun_l*sun;
	sun_l = smoothstep(1.0 - 0.0005 * (6.0f - 5.0f *l),1.0,to_sun) *0.8;
	sky = sky *(1.0 -sun_l) + sun_l*sun;
	float atm = (1.0-l);
	vec3 day =  vec3(atmosphere*(atm)+sky*(1.0 - atm));
	float star_intens = min(0, sun_dir.y);
	float star = smoothstep(0.99994 + 0.008 * star_intens,1.0,Noise3d(normalize(TexCoords.xyz)));
	vec3 night = star * star*vec3(1.0,1.0,1.0);
	//float night_intens = smoothstep(-0.1,0.1, to_sun);
	float night_intens = 1.0 - smoothstep(0.0,0.5,-star_intens + (1.0 - to_sun) *0.08 );
	vec3 sky_color = mix(night,day,night_intens);
	FragColor = vec4(mix(EarthColor * a_earth,sky_color,l_earth),1.0);



}