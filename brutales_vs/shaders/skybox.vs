#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec3 TexCoords;

//uniform mat4 model;
uniform mat4 camera;

void main()
{
    vec4 TexCoords4 = camera * vec4(aPos.xy, 1.0, 1.0);
    //vec4 TexCoords4 = vec4(aPos, 1.0);
    gl_Position = vec4(aPos, 1.0);
    TexCoords = normalize(TexCoords4.xyz);

}
