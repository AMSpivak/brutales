#include "gl_character.h"
#include <sstream>
#include "engine_settings.h"
#include "collision.h"
#include "glm/trigonometric.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "math3d.h"  
std::ostream& operator << ( std::ostream& os, const GlCharacter & character)
{
    os<<"<object>\n";
    character.ToStream(os);
    os<<"<!object>\n";
	return os;
}
// bool GlCharacter::comand_proc_ready = false;

GlCharacter::GlCharacter(CharacterTypes type):
                            engine_frame(0)
                            ,now_frame(0)
                            ,refresh_frame(0)
                            ,current_animation(nullptr)
                            ,m_current_command(AnimationCommand::kNone)
                            ,m_type(type)
                            ,m_is_armed(false)
                            ,m_breed(0)
                            ,m_breed_friendly(false)
                            ,m_dungeon_hero_info(nullptr)
{
    m_brain = Character::CreateBrain(Character::BrainTypes::Empty,[](GlCharacter & character) { return; });
}

GlCharacter::~GlCharacter()
{

}

CharacterTypes GlCharacter::GetType() const
{
    return m_type;
}


void GlCharacter::ToStream(std::ostream& os) const
{
    os<<"name " << GetName()<<"\n";
    
    for(auto &model_name : model_list)
    {
        os<<"model "<<model_name<<"\n";
    }

    for(auto &seq : sequence)
    {
        os<<"sequence "<<seq<<"\n";
    }

    //auto it = std::find_if(sequence.begin(), sequence.end(), [this](std::pair<std::string, AnimationSequence> anim){return *current_animation == anim.second;});
    
    os<<"run_sequence "<<current_animation->m_name<<"\n";

    glm::mat4 tmp_matrix = glm::rotate(model_matrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    os<<"matrix "<<tmp_matrix<<"\n"
    <<"mass_inv "<<mass_inv<<"\n"
    <<"armor "<<GetArmorValue()<<"\n"
    <<"life " << GetLifeValue()<<"\n"
    <<"radius "<<radius<<"\n"
    <<"position "<<GetPosition()<<"\n";

    if(m_is_light)
    {
        os<< "light "<<" "<< m_light_color <<" "<< m_light_position <<" "<< m_light_radius<<"\n";
    }

    if(ghost)
    {
        os<< "ghost\n";
    }

    if(m_breed_friendly)
    {
        os<< "breed "<<m_breed<<"\n";
    }

    for(auto &edge : m_edges)
    {
        os<<"edge "<<edge<<"\n";
    }
}


void UpdateCharacterFromFile(const std::string &filename,GlCharacter & character)
{
    std::ifstream char_file;
	char_file.open(filename);
    std::vector<std::string> lines;
    LoaderUtility::LoadLineBlock(char_file,"never_exists",lines);
    char_file.close(); 
    character.UpdateFromLines(lines); 
}



void GlCharacter::UpdateFromLines(std::vector<std::string> &lines)
{
    if(lines.size()<=1) 
    return;
    
    LoaderUtility::LinesProcessor proc;

    proc.Add("model",[this](std::stringstream &sstream){AddModel(LoaderUtility::GetFromStream<std::string>(sstream));});
    proc.Add("run_sequence",[this](std::stringstream &sstream){UseSequence(LoaderUtility::GetFromStream<std::string>(sstream));});
    proc.Add("ghost",[this](std::stringstream &sstream){ ghost = true;});
    proc.Add("matrix",[this](std::stringstream &sstream){ sstream >> model_matrix;});
    proc.Add("mass_inv",[this](std::stringstream &sstream){ sstream >> mass_inv;});
    proc.Add("armor",[this](std::stringstream &sstream){SetArmorValue(LoaderUtility::GetFromStream<float>(sstream));});
    proc.Add("life",[this](std::stringstream &sstream){SetLifeValue(LoaderUtility::GetFromStream<float>(sstream));});
    proc.Add("name",[this](std::stringstream &sstream){SetName(LoaderUtility::GetFromStream<std::string>(sstream));});
    proc.Add("radius",[this](std::stringstream &sstream){ sstream >> radius;});                                
    proc.Add("edge",[this](std::stringstream &sstream){AddEdge(LoaderUtility::GetFromStream<std::pair<glm::vec3,glm::vec3>>(sstream));});
    proc.Add("position",[this](std::stringstream &sstream){SetPosition(LoaderUtility::GetFromStream<glm::vec3>(sstream));}); 
    proc.Add("weapon",[this](std::stringstream &sstream){sstream >> m_weapon_model >> m_weapon_bone >> m_weapon_base.first >> m_weapon_base.second; m_is_armed = true;}); 
    proc.Add("breed",[this](std::stringstream &sstream){sstream >> m_breed; m_breed_friendly = true;}); 

    proc.Add("light",[this](std::stringstream &sstream)
                                        {
                                            float light_radius = 0.0f;
                                            glm::vec3 color;
                                            glm::vec3 l_position;
                                            sstream >> color >> l_position >> light_radius; 
                                            SetLight(true,color,l_position,light_radius);
                                        });
    proc.Add("orientation",[this](std::stringstream &sstream)
                                    {
                                        float a_x = 0.0f;
                                        float a_y = 0.0f;
                                        float a_z = 0.0f;

                                        sstream >> a_x >> a_y >> a_z; 
                                        
                                        model_matrix = glm::rotate(model_matrix, glm::radians(a_x), glm::vec3(1.0f, 0.0f, 0.0f));
                                        model_matrix = glm::rotate(model_matrix, glm::radians(a_y), glm::vec3(0.0f, 1.0f, 0.0f));
                                        model_matrix = glm::rotate(model_matrix, glm::radians(a_z), glm::vec3(0.0f, 0.0f, 1.0f));
                                    });
    proc.Add("sequence",[this](std::stringstream &sstream)
                                        {
                                            //size_t start = 0;
                                            //size_t end =0;
                                            std::string name;
                                            sstream >> name ;//>> start >> end;
                                             
                                            AnimationSequence seq(name);
                                            sstream >> seq;
                                            AddSequence(name,seq);
                                            UseSequence(name);
                                        });

    proc.Process(lines);
}

void GlCharacter::AddEdge(const std::pair<glm::vec3, glm::vec3> edge)
{
    m_edges.push_back(edge);
}

void GlCharacter::UseSequence(const std::string & name)
{
    current_animation = &sequence[name];
}

void GlCharacter::AddSequence(const std::string & name, const AnimationSequence & in_sequence)
{
    sequence.insert( std::pair<std::string, AnimationSequence>(name,in_sequence));
}

void GlCharacter::Draw(GlScene::Scene &scene) const
{  
    for (auto model : Models)
    {
        /*std::pair<iterator, bool> insert(const value_type & value)
        {
            auto range = std::equal_range(m_values.begin(), m_values.end(), value, m_comparator);
            if (range.first != range.second)
            {
                //range.first.second = value.second;
                return std::make_pair(range.first, false);
            }

            return std::make_pair(m_values.insert(range.first, value), true);
        }*/
        auto range = std::equal_range(scene.model_list.begin(), scene.model_list.end(), model, [](std::shared_ptr<glModel> a, std::shared_ptr<glModel> b) { return *a < *b; });
        scene.model_list.insert(range.second,model);
        //scene.model_list.push_back(model);//model->Draw(scene,now_frame);
    }
}

bool GlCharacter::UseCommand(AnimationCommand command)
{
    try
    {
        std::string jumper = current_animation->jumps.at(command);
        current_animation = &sequence.at(jumper);                        
    }
    catch(const std::out_of_range& exp)
    {
        std::cout << exp.what();
        return false;
        // std::cout<<"Unknown command\n";
    } 
    m_current_command = command;
    return true; 
}


void GlCharacter::Draw(GlScene::Scene &scene,const glm::mat4 &draw_matrix)
{
    unsigned short frame = EngineSettings::GetEngineSettings()->GetFrame();
    if(engine_frame != frame)
    {
      
        for (auto model : Models) model->SetDrawMatrix(draw_matrix);
        Draw(scene);
        engine_frame = frame;
    }
    else
    {
        Draw(scene);
    }
    
}

void GlCharacter::SetDungeonListReference(std::weak_ptr<GlCharacter> this_weak)
{
    m_dungeon_weak_reference = this_weak;
}


void GlCharacter::RefreshMatrixes(float approximation)
{
    bool first_base = true;
    int models_count = Models.size();
    glm::mat4 target_matrix;
    
    for(auto model : Models)
    {
        model->SetFrame(now_frame);

        if(model->parent_idx != -1)
        {
            const auto & parent = Models[model->parent_idx];
            target_matrix = parent->model *
                parent->GetBoneMatrix(now_frame,model->parent_bone) *
                (parent->jub_bones->bones[model->parent_bone].matrix);
            
        }
        else
        {
            if(first_base&&(refresh_frame != now_frame)&&(now_frame!=0))
            {
                glm::mat4 move_matrix = model->GetRotationMatrix(now_frame);
                glm::vec4 move_position = model_matrix * move_matrix[3];
                m_position += glm::vec3(move_position[0],move_position[1],move_position[2]);
                move_matrix[0].w = move_matrix[1].w = move_matrix[2].w = 0.0f;
                move_matrix[3] = glm::vec4(0.0f,0.0f,0.0f,1.0f);
                model_matrix = move_matrix * model_matrix;
                first_base = false;
            }        
            target_matrix = model_matrix;
        }
    
           
        model-> model = target_matrix;       
    }

    if(m_is_armed)
    {
        const auto & mul = Models[m_weapon_model]->GetBoneMatrix(now_frame,m_weapon_bone);
        auto mul_full(glm::inverse(model_matrix) * Models[m_weapon_model]-> model * mul);
        

        m_weapon_old =  m_weapon_now;
        m_weapon_now.first = glm::vec3(mul_full * glm::vec4(m_weapon_base.first,1.0f));
        m_weapon_now.second = glm::vec3(mul_full * glm::vec4(m_weapon_base.second,1.0f));
    }
    refresh_frame = now_frame;
}

void GlCharacter::ExecuteCommand(const std::pair<AnimationCommand,std::string> &command,std::list<std::string> &m_messages)
{
    switch(command.first)
    {
        case AnimationCommand::kMessage:
            m_messages.push_back(command.second);
        break;
        case AnimationCommand::kStrike:
            m_messages.push_back("strike " + GetName() + " " + command.second);
        break;
        case AnimationCommand::kSound:
            m_messages.push_back("sound3d " + GetName() + " " + command.second);
        break;
        default:
        break;
    }
    //m_messages.push_back(current_animation->m_end_message);
}
void GlCharacter::SetBrain(std::shared_ptr<Character::IBrain> brain)
{
    m_brain = brain;
}

void GlCharacter::SetDungeonHeroInfo(DungeonHeroInfo * hero_info)
{
    m_dungeon_hero_info = hero_info;
}

DungeonHeroInfo * GlCharacter::GetDungeonHeroInfo()
{
    return m_dungeon_hero_info;
}
std::weak_ptr<GlCharacter> GlCharacter::GetDungeonListReference()
{
    return m_dungeon_weak_reference;
}

int GlCharacter::Process(std::list<std::string> &m_messages)
{
    if(GetLifeValue() <=0.0f) return 1;
    m_brain->Think(this);

    auto control = now_frame;
    if(current_animation == nullptr)
    {
        now_frame = 0;
    }
    else
    {
        if(now_frame == current_animation->start_frame)
        {
            ExecuteCommand(current_animation->m_start_message,m_messages);
        }

        ++now_frame;

        if(now_frame < current_animation->start_frame) now_frame = current_animation->start_frame;
        if(now_frame > current_animation->end_frame) 
        {
            now_frame = current_animation->m_loop ? current_animation->start_frame:current_animation->end_frame;
            ExecuteCommand(current_animation->m_end_message,m_messages);
            if(current_animation->m_jump)
            {
                current_animation = &sequence[current_animation->m_target_sequence];
                now_frame = current_animation->start_frame;
            }
        }
        
        ExecuteCommand(current_animation->m_frame_message,m_messages);
        
    }

    size_t diff_frame = current_animation->end_frame - current_animation->start_frame > 2 ?now_frame - current_animation->start_frame:4;

    float approx = diff_frame > 3 ? 1.0f : 0.1f + 0.2f * diff_frame;

    if(control != now_frame || IsFocused())
        RefreshMatrixes(approx);
    return 0;
}

DamageReaction GlCharacter::GetDamageReaction()
{
    return current_animation->m_block;
}


DamageReaction GlCharacter::Damage(float damage, const glm::vec3 & from)
{
    glm::vec3 direction;
    glm::vec3 side;
    std::tie(direction, side) = Get2DBasis();
    glm::vec3 event_direction = glm::normalize(from - GetPosition());
    float disorientation = Math3D::Disorientation(direction, event_direction,side);
    const float block_range = 0.5f;
    bool blocked = (current_animation->m_block == DamageReaction::Block || current_animation->m_block == DamageReaction::StrikeBack) && (disorientation < block_range) && (disorientation > -block_range);

        if(!blocked)
        {
            UseCommand(AnimationCommand::kDamaged);
            IGlModel::Damage(damage);
            return DamageReaction::Damage;
        }
        else
        {
            IGlModel::Damage(damage * 0.05f);
            if(current_animation->m_block == DamageReaction::StrikeBack)
            {
                UseCommand(AnimationCommand::kUse);
            }
            return current_animation->m_block;
        }
    
}

void GlCharacter::AddModel(const std::string & name)
{
    model_list.emplace_back(name);
    Models.emplace_back(std::make_shared<glModel>(name));
}

int GlCharacter::AddAxes(std::vector<glm::vec3> &axes)
{
    if(m_edges.empty()) 
        return 0;
    return Collision::AddAxes(axes,m_edges,model_matrix);

    
}

std::pair<float, float> GlCharacter::ProjectOnAxe(const glm::vec3 &axe) const
{
    if(m_edges.empty()) 
    {
        float position_on_axe = glm::dot(axe, m_position);
        std::pair<float, float> ret_value(position_on_axe - radius, position_on_axe + radius);
        return ret_value;
    }
    else
    {
        return Collision::ProjectEdgesOnAxe(model_matrix,m_edges,m_position,axe);
    }
}

std::tuple<glm::vec3, glm::vec3> GlCharacter::Get2DBasis() const
{
    glm::vec4 move_dir =  model_matrix * glm::vec4(0.0f,0.0f,-1.0f,1.0f);
    glm::vec4 move_side = model_matrix * glm::vec4(1.0f,0.0f,0.0f,1.0f);

    return std::make_tuple(move_dir,move_side);
}

bool GlCharacter::IsNoRotateable() const
{
    return current_animation->m_no_rotation;
}

bool GlCharacter::IsFocused() const
{
    return current_animation->m_focus;
}



void GlCharacter::AddEnemy(std::weak_ptr<GlCharacter> enemy)
{
    if(auto finder = enemy.lock())
    {
        // if((m_breed) && (finder->GetBreed() == m_breed))
        // {
        //     return;
        // }
        auto result = std::find_if(enemies_list.begin(),enemies_list.end(),[&](std::pair<std::weak_ptr<GlCharacter>,float> obj){
                    auto candidate = obj.first.lock();
                    return finder == candidate;
                    });
        if(result == enemies_list.end())
        {
            enemies_list.emplace_back(std::make_pair(enemy,1.0f));
            //std::cout<<GetName()<<" hates "<<finder->GetName();
        }
        else
        {
            result->second = 1.0f;
            //std::cout<<GetName()<<" hates again "<<finder->GetName();
        }
        
    }
}

const std::pair<glm::vec3, glm::vec3> GlCharacter::GetWeaponPosition()
{
    return m_weapon_now;
}

const std::pair<glm::vec3, glm::vec3> GlCharacter::GetWeaponPositionOld()
{
    return m_weapon_old;
}

const glm::vec3 & MoveObjectAttempt(IGlModel &object,const glm::vec3 &desired_direction, float length)
{
    if(object.mass_inv > std::numeric_limits<float>::min())
    {
        object.SetPosition(object.GetPosition() + length * desired_direction);
    }
    return object.GetPosition();
}

const unsigned int GlCharacter::GetBreed() const
{
    return m_breed;
}


glm::mat4 RotateToDirection2d(const GlCharacter &character, const glm::vec3 &target_dir, float fit)
{
    float disorientation = 0;               
    glm::vec3 hero_direction;
    glm::vec3 hero_side;
    std::tie(hero_direction, hero_side) = character.Get2DBasis();
    hero_direction[1]= 0;
    hero_side[1]= 0;
    hero_side = glm::normalize(hero_side);
    hero_direction = glm::normalize(hero_direction);

    auto target = glm::normalize(target_dir);
    //constexpr float fit = -45.0f;
    float enemy_disorient = Math3D::Disorientation(hero_direction,target,hero_side);
    return glm::rotate(glm::radians(fit * enemy_disorient), glm::vec3(0.0f, 1.0f, 0.0f)) * character.model_matrix;              
}

AnimationCommand GlCharacter::GetCurrentCommand()
{
    return m_current_command;
}






