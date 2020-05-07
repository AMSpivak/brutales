#version 330 core
layout (location = 0) in vec3 a_Position;


out VS_OUT {
    vec4 screen_normal;
    vec3 normal;
    vec3 position;
} vs_out;

uniform vec4 offset_position;
uniform vec4 map_position;
uniform mat4 camera;


uniform sampler2D HeightMap;


void main()
{
	vec3 position = a_Position.xyz + offset_position.xyz;



	vec2 texCoord = vec2(map_position.x + position.x,map_position.y + position.z) * map_position.z + vec2(0.5,0.5);
	vec2 textureSize = textureSize(HeightMap, 0);
	vec2 texelSize = 1.0 / textureSize;
	vec4 texColor = texture(HeightMap , texCoord);//BiCubic(HeightMap, texCoord,textureSize,texelSize);
	float multiplier = map_position.z;//texelSize.x;//map_position.z * offset_position.w;
	float height_x1 = texture(HeightMap , texCoord + vec2(1.0,0.0)* multiplier).x;
	float height_x2 = texture(HeightMap , texCoord + vec2(-1.0,0.0)* multiplier).x;
	float height_y1 = texture(HeightMap , texCoord + vec2(0.0,1.0)* multiplier).x;
	float height_y2 = texture(HeightMap , texCoord + vec2(0.0,-1.0)* multiplier).x;
	float height = texColor.x;



	vec3 nz = vec3(0.0,(height_y1 - height_y2)* map_position.w,2.0);
	vec3 nx = vec3(2.0,(height_x1 - height_x2)* map_position.w,0.0);



	vs_out.normal=normalize(cross(nz,nx));
	//vs_out.normal=normalize(vec3(0.0,1.0,0.0));
	vs_out.screen_normal=camera * vec4(vs_out.normal,0.0);
	position.y += height * map_position.w;
    vs_out.position = position.xyz;
		
    gl_Position = camera * vec4(position,1.0);
}
