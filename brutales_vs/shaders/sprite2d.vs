#version 330 core
layout (location = 0) in vec4 a_Position;
layout (location = 1) in vec2 a_Tex;

uniform mat4 DrawMatrix;
uniform mat4 SpriteMatrix;

out  vec2 v_Tex;
out  vec2 v_Depth;
void main()
{
   vec4 v_Tex_vec =vec4(a_Tex.xy, 0.0,1.0);
   v_Tex_vec  =  SpriteMatrix*v_Tex_vec;
   vec4 v_Position = DrawMatrix* a_Position;
   v_Tex.xy = v_Tex_vec.xy;
   v_Depth = 0.5 * (v_Position.xy + vec2(1.0,1.0));
   //v_Depth = v_Position.xy;
   
   gl_Position = v_Position;
 }

