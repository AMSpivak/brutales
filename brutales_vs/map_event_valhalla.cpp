#include "map_event_valhalla.h"
#include "collision.h"

void MapEventValhalla::AddEdge(const std::pair<glm::vec3, glm::vec3> edge)
{
    m_edges.push_back(edge);
}

void MapEventValhalla::Show(const glm::vec3 & offset, GlScene::glCamera & camera)
{
    /*renderBillBoardDepth(m_current_shader,m_depthmap,m_texture,   
        m_width,m_height,position,offset,camera);*/
    
    renderBillBoardDepth(m_current_shader,m_depthmap,m_texture,   
            m_width,m_height,glm::vec4(m_alpha,m_alpha,m_alpha,m_alpha),position,offset,camera);

}


int MapEventValhalla::AddAxes(std::vector<glm::vec3> &axes)
{  
    return 0;
}

bool MapEventValhalla::IsLight(glm::vec4 &light_position,glm::vec3 &color)
{
    light_position = glm::vec4(position[0],position[1],position[2],6.0f);
    color = glm::vec3(3.0f,3.0f,3.0f);
    //color = glm::vec3(m_alpha,m_alpha,m_alpha);
    return true;
}


std::pair<float, float> MapEventValhalla::ProjectOnAxe(const glm::vec3 & axe)
{
    return std::pair<float, float>(0.0f,0.0f);
}

EventProcessResult MapEventValhalla::Process()
{
    position.y+=0.03f;
    m_alpha -=m_alpha_dec;
    m_alpha_dec *= 1.05f;

    return m_alpha < 0.0 ? EventProcessResult::Kill: EventProcessResult::Nothing;
}

InteractionResult MapEventValhalla::Interact(std::weak_ptr<GlCharacter> model,std::string &return_value)
{
    return InteractionResult::Nothing;
}