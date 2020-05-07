#include "map_event_hero_action.h"
#include "collision.h"

void IMapEventHeroAction::AddEdge(const std::pair<glm::vec3, glm::vec3> edge)
{
    m_edges.push_back(edge);
}

void IMapEventHeroAction::Show(const glm::vec3 & offset, GlScene::glCamera & camera)
{
    /*renderBillBoardDepth(m_current_shader,m_depthmap,m_texture,   
        m_width,m_height,position,offset,camera);*/
/*
    glm::vec4 norm4;
    glm::vec3 norm;

    for(auto edge :m_edges)
    {
        norm4 = model_matrix * glm::vec4(edge.second[0],edge.second[1],edge.second[2],1.0f);
        norm = position + glm::vec3(norm4[0],norm4[1],norm4[2]);
        renderBillBoardDepth(m_current_shader,m_depthmap,m_texture,   
            m_width,m_height,glm::vec4(1.0,1.0,1.0,1.0),norm,offset,camera);
    }*/

}


int IMapEventHeroAction::AddAxes(std::vector<glm::vec3> &axes)
{
    return Collision::AddAxes(axes,m_edges,model_matrix);
}



std::pair<float, float> IMapEventHeroAction::ProjectOnAxe(const glm::vec3 & axe)
{
    return Collision::ProjectEdgesOnAxe(model_matrix,m_edges,position,axe);
}

EventProcessResult IMapEventHeroAction::Process()
{
    //std::cout<<"Stop event\n";
    return EventProcessResult::Kill;
}

InteractionResult IMapEventHeroAction::Interact(std::weak_ptr<GlCharacter> model,std::string &return_value)
{
    //model.Damage(0.1f);
    //std::cout<<"life "<<model.GetLifeValue()<<"\n";
    model.lock()->UseCommand(m_command);
    //if(model.GetLifeValue()< 0)
    //    return InteractionResult::Kill;
    return InteractionResult::Use;
}