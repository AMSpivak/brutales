#version 330 core
out vec4 FragColor;

uniform sampler2D u_Texture;
uniform vec4 corrector;



in  vec2 v_Tex;
void main()
{
    FragColor =  corrector*(texture2D(u_Texture, v_Tex));
    
}