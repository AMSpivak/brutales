#version 330 core
out vec4 FragColor;

uniform sampler2D u_Texture;
uniform sampler2D DepthMap;
uniform vec4 corrector;

uniform float Depth;

in  vec2 v_Tex;
in  vec2 v_Depth;
void main()
{
    float d = texture2D(DepthMap, v_Depth).x;
    if(d < Depth) discard;
    //float a = step(d,Depth);
    FragColor =  corrector*(texture2D(u_Texture, v_Tex));
    
}