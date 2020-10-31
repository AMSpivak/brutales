#version 330 core
in vec4 v_Position;


layout (location = 0) out float ExpShadow;


void main()
{
	//vec4 texColor = texture(ourTexture, TexCoord);
    //if(texColor.a < 0.1)
    //    discard;
    //v_Position = v_Position * 0.5 + 0.5;

    const float esm_factor = 10.00;
    ExpShadow = exp(esm_factor * v_Position.z);
}
