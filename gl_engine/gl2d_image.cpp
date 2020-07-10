#include "gl2d_image.h"
#include <algorithm>
#include "glresourses.h"
#include "glm/glm.hpp"
#include <loader.h>
#include <gl_resources_manager.h>


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

    std::string GlImage::Load(const std::vector<std::string>& lines, Interface2D& interface)
    {
        std::string ret_value = "";
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


        processor.Add("shader", [this](std::stringstream& sstream)
            {
                std::string tmp_str;
                sstream >> tmp_str;
                m_shader = GetResourceManager()->GetShader(tmp_str);
            });



        processor.Add("name", [this, &ret_value](std::stringstream& sstream)
            {
                sstream >> ret_value;
            });
        

        

        processor.Process(lines);

        return ret_value;
    }

}