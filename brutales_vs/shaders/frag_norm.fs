#version 330 core
in vec3 ourColor;
//in vec3 ourColorView;
in vec3 v_Position;
in vec2 TexCoord;
in mat3 TBN;

layout (location = 0) out vec4 gAlbedoSpec;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gPosition;

uniform vec4 zero_offset;
uniform vec4 color_corrector;
uniform vec4 rzrv_rough_metal_corrector;
uniform sampler2D ourTexture;
uniform sampler2D UtilityTexture;
uniform sampler2D NormalTexture;
void main()
{

	vec4 texColor = texture(ourTexture, TexCoord) * color_corrector;
    if(texColor.a < 0.1)
        discard;

    vec3 utility = texture(UtilityTexture, TexCoord).xyz * rzrv_rough_metal_corrector.xyz;

    vec4 pos = vec4(v_Position,utility.x);
    gPosition = pos;
    // vec3 normal = vec3(0.0,0.0,1.0);//texture(NormalTexture, TexCoord).xyz;
    vec3 normal = texture(NormalTexture, TexCoord).xyz;
    normal = normalize(normal * 2.0 - 1.0); 
    normal = normalize(TBN * normal); 
    //normal = normal * 0.5 + 0.5; 
    
    gNormal = vec4(normal, utility.y);
    //gNormal = vec4(ourColor.xyz, utility.y);
    //gNormal = vec4(normal.xyz, utility.y);
    float val = length(normal);

	// gAlbedoSpec = vec4(normal.xyz, 0.06 + utility.z*0.94);//texColor;
	gAlbedoSpec = vec4(texColor.xyz, 0.06 + utility.z*0.94);//texColor;
	//gAlbedoSpec = vec4(normal, 0.06 + utility.z*0.94);//texColor;
	
	//gAlbedoSpec = gNormal;
}