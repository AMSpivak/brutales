#include "game_event_fabric.h"
#include "map_event_hero_strikes.h"
#include "map_event_hero_action.h"
#include "map_event_valhalla.h"
#include "gl_character.h"
#include "math3d.h"
namespace GameEvents
{
    

    std::shared_ptr<IMapEvent> CreateGameEvent(EventTypes event_type, const void * parameters)
    {
        std::shared_ptr<IMapEvent> retval;
        switch(event_type)
        {
            case EventTypes::HeroStrike:
            {

                auto ptrtoptr = static_cast<const GeneralEventStrike *>(parameters);
                
                auto ptr = ptrtoptr->source;
                auto e_ptr = std::make_shared<IMapEventHeroStrike>(ptrtoptr->current_shader,ptrtoptr->depthmap,ptrtoptr->texture,0.3f,0.3f,ptrtoptr->strike_force);
                e_ptr->model_matrix = ptr->model_matrix;

                auto edge = ptr->GetWeaponPosition();
                auto edge_old = ptr->GetWeaponPositionOld();
                auto second_m = (edge_old.first + edge_old.second) * 0.5f;
                auto fist_m = glm::normalize(edge.first - edge.second);
                edge.first += fist_m * 0.1f;
                edge.second -= fist_m * 0.1f;
                fist_m = glm::normalize((edge.first + edge.second) * 0.5f - second_m);
                second_m -= fist_m * 0.1f;


                e_ptr->SetIndicator(edge.first);

                e_ptr->AssignPoints(edge.first,edge.second,second_m);

                edge.first[1] = 0.0f;
                edge.second[1] = 0.0f;

                edge_old.first[1] = 0.0f;
                edge_old.second[1] = 0.0f;
                glm::vec3 norm_z(0.0f,0.0f,1.0f);
                second_m[1] = 0.0f;
                
                if(!Math3D::IsCounterClockwiseTriangle(edge.first,edge.second,second_m,norm_z))
                {
                    std::swap(edge.first,edge.second);
                }

                e_ptr->AddEdge(edge);        
                e_ptr->AddEdge(std::make_pair(edge.second,second_m));
                e_ptr->AddEdge(std::make_pair(second_m,edge.first));
        

                e_ptr->position = ptr->GetPosition();
                e_ptr->SetOwner(ptr);

                return e_ptr;
            }
            break;
            case EventTypes::HeroUse:
            {
                auto ptr = static_cast<const GlCharacter *>(parameters);
                auto e_ptr = std::make_shared<IMapEventHeroAction>(1.0f,1.4f,AnimationCommand::kUse);
                e_ptr->model_matrix = ptr->model_matrix;
                e_ptr->AddEdge(std::pair<glm::vec3,glm::vec3>(glm::vec3(0.3f,0.0f,-0.5f),glm::vec3(0.5f,0.0f,-2.5f)));
                e_ptr->AddEdge(std::pair<glm::vec3,glm::vec3>(glm::vec3(0.5f,0.0f,-2.5f),glm::vec3(-0.5f,0.0f,-2.5f)));
                e_ptr->AddEdge(std::pair<glm::vec3,glm::vec3>(glm::vec3(-0.5f,0.0f,-2.5f),glm::vec3(-0.3f,0.0f,-0.5f)));
                e_ptr->AddEdge(std::pair<glm::vec3,glm::vec3>(glm::vec3(-0.3f,0.0f,-0.5f),glm::vec3(0.3f,0.0f,-0.5f)));
                e_ptr->position = ptr->GetPosition();
                //e_ptr->position[1] = 0;
                
                return e_ptr;
            }
            break;

            case EventTypes::BarrelValhalla:
            {
                auto ptr = static_cast<const GeneralEventStruct *>(parameters);                
                auto e_ptr = std::make_shared<MapEventValhalla>(ptr->current_shader,ptr->depthmap,ptr->texture,1.0f,1.0f);
                e_ptr->position = ptr->object->GetPosition();
                e_ptr->position.y += 1.5f;
                return e_ptr;
            }
            break;    

            default:
            break;
        }
        return retval;
    }
}