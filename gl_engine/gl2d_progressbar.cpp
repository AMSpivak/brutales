#include "gl2d_progressbar.h"
#include <algorithm>
#include "glresourses.h"
#include "glm/glm.hpp"


namespace Gl2D
{
    void GlProgressbar::Draw()
    {
        RecalculateGeometry();

		constexpr float bar_scaler  = 1.0;

		float m_progress = m_value_function();
        renderSprite(m_shader,
			real_x, real_y,
			real_x + m_progress * real_width * bar_scaler, real_y, 
			real_x + m_progress * real_width * bar_scaler, real_y + real_height,
			real_x, real_y + real_height,
			glm::vec4(1.0f,1.0f,1.0f,1.0f),&(m_fill_texture->m_texture),
			m_progress *bar_scaler, 1.0f,
			0.0f,0.0f
			);
        renderSprite(m_shader,
			real_x, real_y,
			real_x + real_width, real_y, 
			real_x + real_width, real_y + real_height,
			real_x, real_y + real_height,
			glm::vec4(1.0f,1.0f,1.0f,1.0f),&(m_bar_texture->m_texture),
			bar_scaler, 1.0f,
			0.0f,0.0f
			);
    }

	std::string GlProgressbar::Load(const std::vector<std::string>& lines, Interface2D& interface)
	{
		return "";
	}
}