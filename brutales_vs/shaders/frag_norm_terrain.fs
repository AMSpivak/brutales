#version 330 core
in vec3 ourColor;
//in vec3 ourColorView;
in vec3 v_Position;
in vec2 TexCoord;
in mat3 TBN;

layout (location = 0) out vec4 gAlbedoSpec;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gPosition;
uniform vec3 zero_offset;
uniform sampler2D ourTexture;
uniform sampler2D UtilityTexture;
uniform sampler2D NormalTexture;
void main()
{
    float tex_mul = 0.2;
    vec3 t_coord = zero_offset + v_Position;
    vec3 norm_or = normalize(TBN * vec3(0.0,0.0,1.0));

    float mx = step(0.0,norm_or.x)*2.0 -1.0;
    float my = step(0.0,norm_or.y)*2.0 -1.0;
    float mz = step(0.0,norm_or.z)*2.0 -1.0;
    vec2 TexCoord_X = tex_mul * mx * vec2(-t_coord.z,t_coord.y);
    vec2 TexCoord_Y = tex_mul * my * vec2(t_coord.x,-t_coord.z);
    vec2 TexCoord_Z = tex_mul * mz * vec2(t_coord.x,t_coord.y);


    vec3 weight = abs(norm_or);
    weight = (weight - 0.2) * 7;
    weight = normalize(max(weight, 0));      // Force weights to sum to 1.0 (very important!)
    //vec3 weight_summ = 
    ///weight /= (weight.x + weight.y + weight.z ).x; 


    const vec4 pows = vec4(2.2,2.2,2.2,1.0);

    vec4 texColor = weight.x*pow(texture(ourTexture, TexCoord_X),pows) 
                    + weight.y*pow(texture(ourTexture, TexCoord_Y),pows)
                    + weight.z*pow(texture(ourTexture, TexCoord_Z),pows);

    // if(texColor.a < 0.1)
    //     discard;

    vec3 utility = weight.x*texture(UtilityTexture, TexCoord_X).xyz + weight.y*texture(UtilityTexture, TexCoord_Y).xyz + weight.z*texture(UtilityTexture, TexCoord_Z).xyz;
    
    vec4 pos = vec4(v_Position,utility.x);
    gPosition = pos;
    // vec3 normal = vec3(0.0,0.0,1.0);//texture(NormalTexture, TexCoord).xyz;
    vec3 normal_bump_x = normalize((texture(NormalTexture, TexCoord_X).xyz - vec3 (0.5,0.5,0.5))* 2.0);
    vec3 normal_bump_y = normalize((texture(NormalTexture, TexCoord_Y).xyz - vec3 (0.5,0.5,0.5))* 2.0 );
    vec3 normal_bump_z = normalize((texture(NormalTexture, TexCoord_Z).xyz - vec3 (0.5,0.5,0.5))* 2.0 );

    normal_bump_x.y = -normal_bump_x.y;
    normal_bump_y.y = -normal_bump_y.y;
    normal_bump_z.y = -normal_bump_z.y;

    normal_bump_y = my*vec3(normal_bump_y.x, normal_bump_y.z,normal_bump_y.y);
    normal_bump_z = mz*vec3(normal_bump_z.x, normal_bump_z.y, normal_bump_z.z);
    normal_bump_x = mx*vec3(normal_bump_x.z, normal_bump_x.y, -normal_bump_x.x);


    vec3 normal_bump = weight.x*normal_bump_x + weight.y*normal_bump_y + weight.z*normal_bump_z;
    //normal_bump = normal_bump_x;//vec3(normal_bump_y.x,normal_bump_y.z,-normal_bump_y.y);
    //normal_bump = vec3(normal_bump_y.x,normal_bump_y.x,normal_bump_y.x);
    //normal_bump = normalize(normal_bump * 2.0 - vec3 (1.0)); 

    //vec3 normal = normalize(norm_or + 0.3 * normal_bump); 
    //vec3 normal = normalize(normal_bump); 
    vec3 normal = normalize(normal_bump); 
    //normal = normal * 0.5 + 0.5; 
    normal.x = (normal.x + 1.01) * (step(0,normal.z)*2.0 - 1.0);
    normal.z = 0;
    gNormal = vec4(normal, utility.y);
    //gNormal = vec4(ourColor.xyz, utility.y);
    //gNormal = vec4(normal.xyz, utility.y);
    float val = length(normal);

	// gAlbedoSpec = vec4(normal.xyz, 0.06 + utility.z*0.94);//texColor;
	gAlbedoSpec = vec4(texColor.xyz, 0.06 + utility.z*0.94);//texColor;
	//gAlbedoSpec = vec4(normal, 0.06 + utility.z*0.94);//texColor;
	
	//gAlbedoSpec = gNormal;
}