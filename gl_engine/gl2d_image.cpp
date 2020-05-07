#include "gl2d_image.h"
#include <algorithm>
#include "glresourses.h"
#include "glm/glm.hpp"


namespace Gl2D
{
    void GlImage::Draw()
    {
        RecalculateGeometry();


        renderSprite(m_shader,
			real_x, real_y,
			real_x + real_width, real_y, 
			real_x + real_width, real_y + real_height,
			real_x, real_y + real_height,
			glm::vec4(1.0f,1.0f,1.0f,1.0f),&(m_texture->m_texture),
			1.0f, 1.0f,
			0.0f,0.0f
			);
    }
    

    void GlImage::SetImage(sp_texture image)
    {
        m_texture = image;
 
    }

}