#include "map_event_hero_strikes.h"
#include "collision.h"

void IMapEventHeroStrike::AddEdge(const std::pair<glm::vec3, glm::vec3> edge)
{
    m_edges.push_back(edge);
}

void IMapEventHeroStrike::SetIndicator(glm::vec3 indicator)
{
    m_indicator = indicator;
}

void IMapEventHeroStrike::Show(const glm::vec3 & offset, GlScene::glCamera & camera)
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
    //m_edges[0].first
    if(m_texture)
    {
        float m_alpha = 1.0f;

        const glm::vec2 t(0.0f,1.0f);

        auto v = glm::vec3(model_matrix * glm::vec4(m_indicator,1.0));
        auto draw_position = position - offset;
        
         auto d0 =draw_position + glm::vec3(model_matrix * glm::vec4(m_points[0],1.0f));
         auto d1 =draw_position + glm::vec3(model_matrix * glm::vec4(m_points[1],1.0f));
         auto d2 =draw_position + glm::vec3(model_matrix * glm::vec4(m_points[2],1.0f));

        // renderBillBoardDepth(m_current_shader,m_depthmap,m_texture,   
        //     m_width,m_height,glm::vec4(m_alpha,m_alpha,m_alpha,m_alpha),position + v,offset,camera);
        RenderSingleTriangle(m_current_shader, m_depthmap, 
        d0, t, d1, t, d2, t,
        glm::vec4(m_alpha,m_alpha,m_alpha,m_alpha),
        camera,
        m_texture);

        // renderBillBoardDepth(m_current_shader,m_depthmap,m_texture,   
        //      m_width,m_height,glm::vec4(m_alpha,m_alpha,m_alpha,m_alpha),position + m_points[0],offset,camera);
       
        // renderBillBoardDepth(m_current_shader,m_depthmap,m_texture,   
        //      m_width,m_height,glm::vec4(m_alpha,m_alpha,m_alpha,m_alpha),position + m_points[1],offset,camera);
        // renderBillBoardDepth(m_current_shader,m_depthmap,m_texture,   
        //      m_width,m_height,glm::vec4(m_alpha,m_alpha,m_alpha,m_alpha),position + m_points[2],offset,camera);
       
    }
}

void IMapEventHeroStrike::AssignPoints( const glm::vec3 & p1,
                                        const glm::vec3 & p2,
                                        const glm::vec3 & p3)
{
    m_points[0] = p1;
    m_points[1] = p2;
    m_points[2] = p3;
}

int IMapEventHeroStrike::AddAxes(std::vector<glm::vec3> &axes)
{
    return Collision::AddAxes(axes,m_edges,model_matrix);
}



std::pair<float, float> IMapEventHeroStrike::ProjectOnAxe(const glm::vec3 & axe)
{
    return Collision::ProjectEdgesOnAxe(model_matrix,m_edges,position,axe);
}

EventProcessResult IMapEventHeroStrike::Process()
{
    //std::cout<<"Stop event\n";
    return EventProcessResult::Kill;
}

InteractionResult IMapEventHeroStrike::Interact(std::weak_ptr<GlCharacter> model,std::string &return_value)
{   
    if(auto pmodel = model.lock())
    {
        if(auto powner = m_owner.lock())
        {
            auto m_breed = pmodel->GetBreed();
            
            if((m_breed) && (powner->GetBreed() == m_breed))
            {
                return InteractionResult::Nothing;
            }

            auto damage_reaction = pmodel->Damage(m_damage,GetPosition());

            if(pmodel->GetType() == CharacterTypes::mob)
            {
                pmodel->AddEnemy(m_owner);
            }

            if( (powner->GetType() == CharacterTypes::hero)&&
                (pmodel->GetType() == CharacterTypes::mob)
            )
            {
                powner->AddEnemy(model);
            }

            if(damage_reaction == DamageReaction::StrikeBack)
            {
                powner->UseCommand(AnimationCommand::kStrikeBlocked);
                std::cout<<powner->GetName()<<" blocked!\n";
            }
        }
        //std::cout<<model.GetName()<<" life "<<model.GetLifeValue()<<"\n";
        if((pmodel->GetLifeValue()< 0)&&(pmodel->GetType() != CharacterTypes::hero))
        {
            return InteractionResult::Kill;
        }
        return InteractionResult::Damage;
    }
    else
    {
        return InteractionResult::Nothing;
    }
    
}

bool IMapEventHeroStrike::IsInteractable(std::weak_ptr<GlCharacter> obj)
{
    //if(m_owner.expired)
    return m_owner.lock() != obj.lock();
}