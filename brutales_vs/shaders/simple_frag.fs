#version 330 core
out vec4 FragColor;

uniform sampler2D u_Texture;
uniform sampler2D DepthMap;
uniform vec4 corrector;

in  vec2 Tex;
in  vec3 v_Depth;
void main()
{
    //if(texture2D(DepthMap, v_Depth).x < Depth.z) discard;
    //FragColor =  corrector*(texture2D(u_Texture, Tex));
    float a = 0.1;
    FragColor =  vec4(1.0,0.03,0.0,1.0) *a; 
}