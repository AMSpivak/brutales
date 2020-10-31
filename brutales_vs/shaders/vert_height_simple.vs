#version 330 core
layout (location = 0) in vec3 a_Position;


out vec4 v_Position;


uniform vec4 offset_position;
uniform vec4 map_position;
uniform mat4 camera;

uniform sampler2D HeightMap;


void main()
{
	vec3 position = a_Position.xyz + offset_position.xyz;



	vec2 texCoord = vec2(map_position.x + position.x,map_position.y + position.z) * map_position.z + vec2(0.5,0.5);
	vec4 texColor = texture(HeightMap , texCoord);//BiCubic(HeightMap, texCoord,textureSize,texelSize);
	position.y += texColor.x * map_position.w;
    v_Position = camera * vec4(position.xyz,1.0);
		
    gl_Position = v_Position;

}
