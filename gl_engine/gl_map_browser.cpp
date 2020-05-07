#include "gl_map_browser.h"
#include <algorithm>
#include "glresourses.h"
#include "glm/glm.hpp"


namespace Gl2D
{
    void GlMapBrowser::Draw()
    {
        RecalculateGeometry();
        float tile_size = 0.5f * m_zoom;

        float x_off = 0.5f - m_map_x - tile_size;
        float y_off = 0.5f - m_map_y - tile_size;
        tile_size = m_zoom;
        renderSprite(m_shader,
			real_x, real_y,
			real_x + real_width, real_y, 
			real_x + real_width, real_y + real_height,
			real_x, real_y + real_height,
			m_main_image_corrector,
            &(m_map_texture->m_texture),
			tile_size, tile_size,
			x_off,y_off
			);

        glEnable(GL_ALPHA_TEST);
        glEnable(GL_BLEND);	
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        if(m_blured_map_texture)
        {
            float focus = glm::abs(m_map_z)/m_zoom;
            focus = glm::clamp(focus ,0.0f,1.0f);

            renderSprite(m_shader,
                real_x, real_y,
                real_x + real_width, real_y, 
                real_x + real_width, real_y + real_height,
                real_x, real_y + real_height,
                glm::vec4(focus,focus,focus,focus),&(m_blured_map_texture->m_texture),
                tile_size, tile_size,
                x_off,y_off
                );
        }
        
        if(m_crosshair_texture)
        {
            float crosshair_size_x = real_width *0.2f;
            float crosshair_size_y = real_height *0.2f;
            float crosshair_x = real_x + real_width*0.5f - crosshair_size_x *0.5f;
            float crosshair_y = real_y + real_height*0.5f - crosshair_size_y *0.5f;
            renderSprite(m_shader,
			crosshair_x, crosshair_y,
			crosshair_x + crosshair_size_x, crosshair_y, 
			crosshair_x + crosshair_size_x, crosshair_y + crosshair_size_y,
			crosshair_x, crosshair_y + crosshair_size_y,
			glm::vec4(1.0f,1.0f,1.0f,1.0f),&(m_crosshair_texture->m_texture),
			1.0f, 1.0f,
			0.0f,0.0f
			);
        }

        glEnable(GL_CULL_FACE);
        glDisable(GL_ALPHA_TEST);
        glDisable(GL_BLEND);
    }

    const glm::vec4 GlMapBrowser::GetCorrector()
    {
        return m_main_image_corrector;
    }

    void GlMapBrowser::SetCorrector(const glm::vec4 && value)
    {
        m_main_image_corrector = value;
    }

    void GlMapBrowser::SetZoom(float value)
    {
        if(value > 0.001f && value < 1000.f)
        {
            m_zoom = 1.0/value;
        }
    }

    void GlMapBrowser::SetFocus(float z)
    {
        m_map_z = z;
    }

    const float GlMapBrowser::GetFocus()
    {
        return m_map_z;
    }

    const float GlMapBrowser::GetZoom()
    {
        return 1.0/m_zoom;
    }

    void GlMapBrowser::MoveMap(float x, float y)
    {
        m_map_x -= m_zoom * x;
        m_map_x = glm::clamp(m_map_x,-0.5f,0.5f);
        m_map_y -= m_zoom * y;
        m_map_y = glm::clamp(m_map_y,-0.5f,0.5f);
    }

    const float GlMapBrowser::GetX()
    {
        return m_map_x;
    }


    const float GlMapBrowser::GetY()
    {
        return m_map_y;        
    }
}