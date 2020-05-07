#include "gl_font16x16.h"
#include <iostream>


GlText16x16::GlText16x16(const std::string & filename, GLAtlas<IGlTextureStruct> &m_texture_atlas,float width, float height)
    :IGlText( filename, m_texture_atlas,width, height)
{
	texture = m_texture_atlas.Assign(filename);
}

float GlText16x16::GetStringLength(const std::string &value)
{
	return m_width * 0.6 * value.length();
}


void GlText16x16::DrawString(const std::string &value,float x,float y, GLuint shader)
{
	// glClear(GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    // glEnable(GL_ALPHA_TEST);
    // glEnable(GL_BLEND);	
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// glDisable(GL_CULL_FACE);
	
	const float tile_size = 0.0625f;

	for(const char& chr : value)
	{
		unsigned char y_off = chr >> 4;
		unsigned char x_off = chr & 0x0f;
		renderSprite(shader,
			x, y,
			x + m_width, y, 
			x + m_width, y + m_height,
			x, y + m_height,
			glm::vec4(1.0f,1.0f,1.0f,1.0f),&(texture->m_texture),
			tile_size, tile_size,
			tile_size * x_off, tile_size *y_off
			);
		x += m_width * 0.6;
	}

	// glEnable(GL_CULL_FACE);
	// glDisable(GL_ALPHA_TEST);
    // glDisable(GL_BLEND);	


}
