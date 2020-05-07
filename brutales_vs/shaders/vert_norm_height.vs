#version 330 core
layout (location = 0) in vec3 a_Position;

out vec3 ourColor;
out vec3 v_materials;
out vec3 v_Position;
out vec2 TexCoord;
out mat3 TBN;
//


uniform vec4 offset_position;
uniform vec4 map_position;
uniform mat4 camera;
//uniform mat4 projection;
//uniform mat4    u_BoneMatrices[128];

uniform sampler2D HeightMap;

// float Triangular( float f )
// {
// 	f = f / 2.0;
// 	if( f < 0.0 )
// 	{
// 		return ( f + 1.0 );
// 	}
// 	else
// 	{
// 		return ( 1.0 - f );
// 	}
// 	return 0.0;
// }

// vec4 BiCubic( sampler2D textureSampler, vec2 TexCoord, vec2 TextureSize, vec2 TexelSize )
// {
//     //float texelSizeX = 1.0 / fWidth; //size of one texel 
//     //float texelSizeY = 1.0 / fHeight; //size of one texel 
//     vec4 nSum = vec4( 0.0, 0.0, 0.0, 0.0 );
//     vec4 nDenom = vec4( 0.0, 0.0, 0.0, 0.0 );
//     float a = fract( TexCoord.x * TextureSize.x ); // get the decimal part
//     float b = fract( TexCoord.y * TextureSize.y ); // get the decimal part
// 	float height[16];
// 	int addr = 0;
// 	vec3 normal_summ = vec3(0.0,0.0,0.0);
//     for( int m = -1; m <=2; m++ )
//     {
//         for( int n =-1; n<= 2; n++)
//         {
// 			vec4 vecData = texture2D(textureSampler, 
//                                TexCoord + vec2(TexelSize.x * float( m ), 
// 					TexelSize.y * float( n )));

			
// 			float f  = Triangular( float( m ) - a );
// 			vec4 vecCooef1 = vec4( f,f,f,f );
// 			float f1 = Triangular ( -( float( n ) - b ) );
// 			vec4 vecCoeef2 = vec4( f1, f1, f1, f1 );
// 			normal_summ +=vec3(float( m ) - a,1.0,-( float( n ) - b ));
			
//             nSum = nSum + ( vecData * vecCoeef2 * vecCooef1  );
//             nDenom = nDenom + (( vecCoeef2 * vecCooef1 ));
// 			probes[probe_position] = vec3(vecData.x,f,f1);
// 			//probes[probe_position] = vec3(vecData.x,1.0,1.0);
// 			probe_position++;
//         }
// 		//probe_position++;
//     }
// 	vec4 average = nSum / nDenom;
// 	ourColor = normalize(vec3(0.0,1.0,0.0));
// 	//ourColor = normalize(normal_summ);
//     return average;
// }

// vec4 tex2DBiLinear( sampler2D textureSampler_i, vec2 texCoord_i, vec2 TextureSize, vec2 TexelSize )
// {
//     vec4 p0q0 = texture2D(textureSampler_i, texCoord_i);
//     vec4 p1q0 = texture2D(textureSampler_i, texCoord_i + vec2(TexelSize.x, 0));

//     vec4 p0q1 = texture2D(textureSampler_i, texCoord_i + vec2(0, TexelSize.y));
//     vec4 p1q1 = texture2D(textureSampler_i, texCoord_i + vec2(TexelSize.x , TexelSize.y));

//     float a = fract( texCoord_i.x * TextureSize.x ); // Get Interpolation factor for X direction.
// 					// Fraction near to valid data.

//     vec4 pInterp_q0 = mix( p0q0, p1q0, a ); // Interpolates top row in X direction.
//     vec4 pInterp_q1 = mix( p0q1, p1q1, a ); // Interpolates bottom row in X direction.

//     float b = fract( texCoord_i.y * TextureSize.y );// Get Interpolation factor for Y direction.
// 	vec4 ret = mix( pInterp_q0, pInterp_q1, b ); // Interpolate in Y direction.
	
// 	ourColor = normalize(vec3(pInterp_q0.x-ret.x ,0.005,mix( p0q0, p0q1, b ).x-ret.x));
    
// 	return ret;
	
// }

void main()
{
	vec3 position = a_Position.xyz + offset_position.xyz;



	vec2 texCoord = vec2(map_position.x + position.x,map_position.y + position.z) * map_position.z + vec2(0.5,0.5);
	vec2 textureSize = textureSize(HeightMap, 0);
	vec2 texelSize = 1.0 / textureSize;
	vec4 texColor = texture(HeightMap , texCoord);//BiCubic(HeightMap, texCoord,textureSize,texelSize);
	v_materials = vec3(texColor.y,texColor.z,texColor.w);
	//v_materials = normalize(vec3(texColor.y,0.3,texColor.w));
	float multiplier = map_position.z;//texelSize.x;//map_position.z * offset_position.w;
	float height_x1 = texture(HeightMap , texCoord + vec2(1.0,0.0)* multiplier).x;
	float height_x2 = texture(HeightMap , texCoord + vec2(-1.0,0.0)* multiplier).x;
	float height_y1 = texture(HeightMap , texCoord + vec2(0.0,1.0)* multiplier).x;
	float height_y2 = texture(HeightMap , texCoord + vec2(0.0,-1.0)* multiplier).x;
	float height = texColor.x;

	//float gz = map_position.w *(height_y2 - height_y1);
	//float gx = map_position.w *(height_x2 - height_x1);

	vec3 nz = vec3(0.0,(height_y1 - height_y2)* map_position.w,2.0);
	vec3 nx = vec3(2.0,(height_x1 - height_x2)* map_position.w,0.0);
	ourColor = normalize(cross(nz,nx));

	//position.y += 0.1f;//height * map_position.w;
	position.y += height * map_position.w;
    v_Position = position.xyz;



    vec3 tangent = vec3(vec4(0.0,0.0,-1.0, 0.0));
	tangent = normalize(tangent - dot(tangent, ourColor) * ourColor);
	vec3 B = cross(ourColor, tangent);

	TBN = mat3(tangent, B, ourColor);
		
    gl_Position = camera * vec4(v_Position,1.0);

    TexCoord.x = texCoord.x;
	TexCoord.y = 1.0 - texCoord.y;
}
