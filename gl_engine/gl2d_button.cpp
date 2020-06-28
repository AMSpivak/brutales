#include "gl2d_button.h"
#include <algorithm>
#include "glresourses.h"
#include "glm/glm.hpp"
#include "../gl_engine/loader.h"
#include "gl_resources_manager.h"

namespace Gl2D
{
    void GlButton::Load(const std::vector<std::string>& lines)
    {
        LoaderUtility::LinesProcessor processor;
        processor.Add("geometry", [this](std::stringstream& sstream)
            {
                sstream >> m_x >> m_y >> m_width >> m_height;

            });

        processor.Add("image", [this](std::stringstream& sstream)
            {
                std::string tmp_str;
                sstream >> tmp_str;
                m_texture = GetResourceManager()->m_texture_atlas.Assign(tmp_str);
            });

        processor.Add("image_active", [this](std::stringstream& sstream)
            {
                std::string tmp_str;
                sstream >> tmp_str;
                m_texture_active = GetResourceManager()->m_texture_atlas.Assign(tmp_str);
            });

        processor.Add("shader", [this](std::stringstream& sstream)
            {
                std::string tmp_str;
                sstream >> tmp_str;
                m_shader = GetResourceManager()->GetShader(tmp_str);
            });

        processor.Add("text", [this](std::stringstream& sstream)
            {
                std::string tmp_str;
                sstream >> tmp_str;
                m_text = tmp_str;
            });
        /*m_texture(texture), m_texture_active(texture_active)
            , m_font(font), m_text(text)
            , m_shader(shader), m_active_mul(1.0f)
            , m_action(action)*/

        processor.Process(lines);

    }

    void GlButton::Draw()
    {
        RecalculateGeometry();


        float w = real_width;
        float h = real_height;
        float x = real_x;
        float y = real_y;

        if(m_active)
        {
            x +=0.5f * (1.0f - m_active_mul) * real_width ;
            y +=0.5f * (1.0f - m_active_mul) * real_height;// * m_aspect_ratio;
            w *= m_active_mul ;
            h *= m_active_mul ;
        }

        auto texture = m_active ? &(m_texture_active->m_texture) : &(m_texture->m_texture);
        renderSprite(m_shader,
			x, y,
			x + w, y, 
			x + w, y + h,
			x, y + h,
			glm::vec4(1.0f,1.0f,1.0f,1.0f),texture,
			1.0f, 1.0f,
			0.0f,0.0f
			);

        const float text_size_x = 0.12f * w;
        const float text_size_y = 0.5f * h;

        m_font->SetTextSize(text_size_x,text_size_y);
        m_font->DrawString(m_text,x + (w  - m_font->GetStringLength(m_text)) * 0.5f,y + (h  - text_size_y) * 0.5f/* * 1.2f*/, m_shader);
    }
    

    std::weak_ptr<Gl2dItem> GlButton::ProcessInput(Inputs::InputCommands input)
    {
        if(input == Inputs::InputCommands::Strike)
        {
            m_action();
            return std::weak_ptr<Gl2dItem>();
        }
        else
        {
            return Gl2dItem::ProcessInput(input);
        }
        
    }



}