#include "map_event_general.h"
#include "collision.h"

void MapEventGeneral::AddEdge(const std::pair<glm::vec3, glm::vec3> edge)
{
    m_edges.push_back(edge);
}

void MapEventGeneral::Show(const glm::vec3 & offset, GlScene::glCamera & camera)
{
    /*renderBillBoardDepth(m_current_shader,m_depthmap,m_texture,   
        m_width,m_height,position,offset,camera);*/
    
   /* renderBillBoardDepth(m_current_shader,m_depthmap,m_texture,   
            m_width,m_height,glm::vec4(m_alpha,m_alpha,m_alpha,m_alpha),position,offset,camera);
*/
}

void MapEventGeneral::SetMessage(std::string message)
{
    m_message = message;
}

int MapEventGeneral::AddAxes(std::vector<glm::vec3> &axes)
{  
    return 0;
}

bool MapEventGeneral::IsLight(glm::vec4 &light_position,glm::vec3 &color)
{
    light_position = glm::vec4(position[0],position[1],position[2],6.0f);
    color = glm::vec3(3.0f,3.0f,3.0f);
    //color = glm::vec3(m_alpha,m_alpha,m_alpha);
    return false;//true;
}


std::pair<float, float> MapEventGeneral::ProjectOnAxe(const glm::vec3 & axe)
{
    float position_on_axe = glm::dot(axe, position);
    //std::cout << position<< "\n";
    std::pair<float, float> ret_value(position_on_axe - radius, position_on_axe + radius);

    return ret_value;
}

EventProcessResult MapEventGeneral::Process()
{
    return EventProcessResult::Nothing;
}

InteractionResult MapEventGeneral::Interact(std::weak_ptr<GlCharacter> model,std::string &return_value)
{
    //std::cout<<m_message<<"\n";
    return_value = m_message;
    return InteractionResult::PostMessage;
}