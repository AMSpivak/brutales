#include "brain.h"

#include "gl_character.h"
#include <random>
#include <tuple>
#include <vector>


#include "glm/glm.hpp"

#include "glm/trigonometric.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "math3d.h"
#include "loader.h"
//#include <GLFW/glfw3.h>
namespace Character
{

    bool IBrain::LoadBrain(const std::string &filename)
    {
        std::ifstream brain_stream;
        brain_stream.open(filename); 
        // std::cout<<"Level:"<<filename<<" "<<(level_file.is_open()?"-opened":"-failed")<<"\n"
        if(!brain_stream.is_open())
        {
            brain_stream.close();
            return false;
        }
        auto res = LoadBrain(brain_stream);
        brain_stream.close();
        return res;
    }

    constexpr float attacker_time = 0.5f;
    class BrainEmpty: public IBrain
    {
        public:
        virtual bool LoadBrain(std::istream &is) {return false;}
        virtual void UpdateFromLines(std::vector<std::string> &lines){}
    };

    bool HatesLess(const std::pair<std::weak_ptr<GlCharacter>,float> a,const std::pair<std::weak_ptr<GlCharacter>,float> b, const GlCharacter & character)
    {
        auto a_ptr = a.first.lock();
        if(!a_ptr)
        {
            return false;
        }
        auto b_ptr = b.first.lock();
        if(!b_ptr)
        {
            return false;
        }

        auto l_a = character.GetPosition() - a_ptr->GetPosition();
        auto l_b = character.GetPosition() - b_ptr->GetPosition();

        float d_a = glm::dot(l_a,l_a);
        if(d_a < 0.001f)
            return true;

        float d_b = glm::dot(l_b,l_b);
        if(d_b < 0.001f)
            return false; 

        return a.second/d_a < b.second/d_b;
    }


    bool HeroChoiseLess(std::pair<std::weak_ptr<GlCharacter>,float> &a, std::pair<std::weak_ptr<GlCharacter>,float> &b, const GlCharacter & character)
    {
        auto a_ptr = a.first.lock();
        if(!a_ptr)
        {
            return false;
        }
        auto b_ptr = b.first.lock();
        if(!b_ptr)
        {
            return false;
        }

        auto l_a = character.GetPosition() - a_ptr->GetPosition();
        auto l_b = character.GetPosition() - b_ptr->GetPosition();

        glm::vec3 hero_direction;
        glm::vec3 hero_side;
        std::tie(hero_direction, hero_side) = character.Get2DBasis();
        
        float d_a = glm::dot(l_a,l_a);
        a.second = d_a;
        float d_ac = glm::dot(l_a,hero_direction);

        float d_b = glm::dot(l_b,l_b);
        b.second = d_b;
        float d_bc = glm::dot(l_b,hero_direction);
    
        return (d_ac) > (d_bc);
    }


    
    class BrainHero: public IBrain
    {
        public:
        BrainHero(std::function<void(GlCharacter & character)> world_reaction)
        {
            m_world_reaction = world_reaction;
        }
        virtual void Think(GlCharacter * character) 
        {
            constexpr float enemy_keep_range = 7.0f;
            constexpr float enemy_keep_range_2 = enemy_keep_range * enemy_keep_range;
            m_world_reaction(*character);

            if(!character->enemies_list.empty())
            {
                auto enemy_it = std::max_element(   character->enemies_list.begin(),
                                                    character->enemies_list.end(),
                                                    [&](std::pair<std::weak_ptr<GlCharacter>,float> a,std::pair<std::weak_ptr<GlCharacter>,float> b)->bool
                                                    {
                                                        return HeroChoiseLess(a,b,*character);
                                                    });
                
                
                if((enemy_it->second < enemy_keep_range_2)&&!enemy_it->first.expired() && (enemy_it->first.lock()->GetLifeValue() > 0.0f))
                {
                    character->arch_enemy = enemy_it->first;
                }
                else
                {
                    character->enemies_list.erase(enemy_it);
                }  
            }



            if(auto arch = character->arch_enemy.lock())
            {

                auto arch_distance_vec = character->GetPosition() - arch->GetPosition();
                float arch_distance = glm::dot(arch_distance_vec,arch_distance_vec);
                if(arch_distance > enemy_keep_range_2)
                {
                    character->arch_enemy.reset();
                }
            }

            
            switch(character->GetDamageReaction())
            {
                case DamageReaction::Block:
                case DamageReaction::StrikeBack:
                {
                    auto p_d_info = character->GetDungeonHeroInfo();
                    if(p_d_info && !p_d_info->attackers.empty())
                    {
                        auto attacker = p_d_info->attackers.front().second;
                        if(auto p_attacker = attacker.lock())
                        {
                            auto arch_distance_vec = character->GetPosition() - p_attacker->GetPosition();
                            float arch_distance = glm::dot(arch_distance_vec,arch_distance_vec);
                            if(arch_distance < enemy_keep_range_2)
                                character->arch_enemy = attacker;
                        }
                    }
                }
                break;
                default:
                break;
            }
        }

        virtual bool LoadBrain(std::istream &is) {return false;}
        virtual void UpdateFromLines(std::vector<std::string> &lines){}
    };

    
    

    class BrainMob: public IBrain
    {
        private:
        float distance;

        float step_back_distance;
        float walk_distance;
        float attak_distance;

        std::vector<glm::vec3>::iterator m_current_point;

        std::vector<glm::vec3> m_track_points;

        bool FitDistance(GlCharacter* character, float enemy_distance, float fast_forward, float forward, float back)
        {
            if (enemy_distance > fast_forward)
            {
                character->UseCommand(AnimationCommand::kFastMove);   /* code */
                return true;
            }

            if (enemy_distance > forward)
            {
                character->UseCommand(AnimationCommand::kMove);   /* code */
                return true;
            }
            
            if (enemy_distance < back)
            {
                character->UseCommand(AnimationCommand::kStepBack);   /* code */
                return true;
            }

            return false;
        }

        void FitDirection(GlCharacter* character, const glm::vec3& enemy_vector, const float fit)
        {
            if (!character->IsNoRotateable())
            {
                character->model_matrix = RotateToDirection2d(*character, enemy_vector, fit);
            }
        }

        void SimpleAttack(GlCharacter* character, float enemy_distance, const glm::vec3 &enemy_vector,  int dice_roll)
        {
            constexpr float fit = -45.0f;
            FitDirection(character, enemy_vector, fit);

            if (!FitDistance(character, enemy_distance, walk_distance, attak_distance, step_back_distance))
            {
                if (dice_roll > (random_maximum - 50))
                {
                    character->UseCommand(AnimationCommand::kStrike);
                }
                else
                {
                    character->UseCommand(AnimationCommand::kStance);
                }
            }
        }

        void HeroAttack(GlCharacter* character, float enemy_distance, const glm::vec3& enemy_vector, int dice_roll)
        {
            constexpr float fit = -45.0f;
            FitDirection(character, enemy_vector, fit);

            auto p_d_info = character->GetDungeonHeroInfo();

            auto element = std::make_pair(p_d_info->now_time, character->GetDungeonListReference());
            if (!p_d_info->attackers.empty())
            {
                auto attacker_it = p_d_info->FindInAttackers(element);
                if (attacker_it != p_d_info->attackers.end())
                {
                    if (!FitDistance(character, enemy_distance, walk_distance, attak_distance, step_back_distance) && (p_d_info->attackers.cbegin() == attacker_it))
                    {
                        double wait_time = p_d_info->now_time - p_d_info->attaker_time;
                        if (wait_time > attacker_time * 3)
                        {
                            if (attacker_it->first > attacker_time)
                            {
                                if (character->UseCommand(AnimationCommand::kStrike))
                                {
                                    attacker_it->first = 0;
                                    p_d_info->attaker_time = p_d_info->now_time;
                                }
                            }
                        }
                    }
                    else
                    {
                        character->UseCommand(AnimationCommand::kStepRight);
                    }

                    return;
                }
            }

            const int attackers_max = 3;
            if (p_d_info->attackers.size() < attackers_max)
            {
                auto element = std::make_pair(0, character->GetDungeonListReference());
                auto res = p_d_info->FindInAttackers(element);
                if (res == p_d_info->attackers.end())
                {
                    p_d_info->attackers.push_back(element);
                }
            }
            else
            {
                if (!FitDistance(character, enemy_distance, walk_distance + attak_distance, walk_distance, attak_distance))
                {
                    character->UseCommand(AnimationCommand::kStepRight);
                }

            }  
        }

        void FreeRun(GlCharacter* character, std::uniform_int_distribution<int> &distribution)
        {
            auto dice_roll = distribution(random_generator);
            if (dice_roll > random_maximum - 1)
            {
                dice_roll = distribution(random_generator);
                if (dice_roll > (random_maximum / 2))
                {
                    if (dice_roll > (5 * random_maximum / 8))
                    {
                        character->UseCommand(AnimationCommand::kMove);
                    }
                    else
                    {
                        character->UseCommand(AnimationCommand::kFastMove);
                    }
                }
                else
                {
                    if (dice_roll > (random_maximum / 4))
                    {
                        character->UseCommand(AnimationCommand::kStance);
                        //character.UseSequence("stance");
                    }
                }
            }


            if (m_track_points.empty())
            {
                if (rotator == 0)
                {
                    dice_roll = distribution(random_generator);

                    if (dice_roll > (random_maximum - 50))
                    {
                        rotator = distribution(random_generator) - random_maximum / 2;
                    }
                }
                else
                {
                    int sign = 1;
                    if (rotator > 0)
                    {
                        --rotator;
                    }
                    else
                    {
                        ++rotator;
                        sign = -1;
                    }
                    character->model_matrix = glm::rotate(character->model_matrix, glm::radians(sign * 0.1f), glm::vec3(0.0f, 1.0f, 0.0f));
                }
            }
            else
            {
                if (character->GetCurrentCommand() == AnimationCommand::kStance)
                {
                    if (dice_roll > random_maximum - random_maximum / 100)
                    {
                        character->UseCommand(AnimationCommand::kMove);
                    }
                }
                else
                {
                    auto position = character->GetPosition();
                    position[1] = 0.0f;
                    position = *m_current_point - position;
                    float enemy_distance = glm::length(position);

                    constexpr float fit = -45.0f;
                    character->model_matrix = RotateToDirection2d(*character, position, fit);

                    if (enemy_distance > walk_distance)
                    {
                        character->UseCommand(AnimationCommand::kFastMove);
                    }
                    else if (enemy_distance < attak_distance)
                    {
                        if ((++m_current_point) == m_track_points.end())
                        {
                            m_current_point = m_track_points.begin();
                        }
                        character->UseCommand(AnimationCommand::kStance);
                    }
                    else
                    {
                        character->UseCommand(AnimationCommand::kMove);
                    }
                }

            }
        }


        public:
        BrainMob(std::function<void(GlCharacter & character)> world_reaction):  rotator(0), 
                                                                                distance(0.0f),
                                                                                step_back_distance(2.0f),
                                                                                walk_distance(7.0f),
                                                                                attak_distance(3.3f)
        {
            m_world_reaction = world_reaction;
        }
        virtual void Think(GlCharacter * character) 
        {
            m_world_reaction(*character);

            if(!character->enemies_list.empty())
            {
                auto enemy_it = std::max_element(   character->enemies_list.begin(),
                                                    character->enemies_list.end(),
                                                    [&](std::pair<std::weak_ptr<GlCharacter>,float> a,std::pair<std::weak_ptr<GlCharacter>,float> b)->bool
                                                    {
                                                        return HatesLess(a,b,*character);
                                                    });
                
                if((enemy_it->second > 0.0f) && !enemy_it->first.expired() && (enemy_it->first.lock()->GetLifeValue() > 0.0f))
                {
                    character->arch_enemy = enemy_it->first;
                }
                else
                {
                    character->enemies_list.erase(enemy_it);
                }
            }


            std::uniform_int_distribution<int> distribution(1,random_maximum);
            int dice_roll = distribution(random_generator);
            
            if(character->arch_enemy.expired())
            {
                FreeRun(character, distribution);
            }
            else  if(auto enemy = character->arch_enemy.lock())
            {
                glm::vec3 enemy_vector = enemy->GetPosition() - character->GetPosition();
                float enemy_distance = glm::length(enemy_vector);
                constexpr float distance_smooth = 0.1f;
                distance = enemy_distance * distance_smooth + (1.0f - distance_smooth) * distance;
                enemy_distance = distance;

                auto p_d_info = character->GetDungeonHeroInfo();

                dice_roll = distribution(random_generator);


                if (p_d_info->hero.lock() == character->arch_enemy.lock())
                {
                    HeroAttack(character, enemy_distance, enemy_vector, dice_roll);

                }
                else
                {
                    SimpleAttack(character, enemy_distance, enemy_vector, dice_roll);
                }
            }
        }

        virtual void UpdateFromLines(std::vector<std::string> &lines)
        {
            LoaderUtility::LinesProcessor proc;
            proc.Add("step_back_distance",[this](std::stringstream &sstream){ sstream >> step_back_distance;});
            proc.Add("walk_distance",[this](std::stringstream &sstream){ sstream >> walk_distance;});
            proc.Add("attak_distance",[this](std::stringstream &sstream){ sstream >> attak_distance;});
            proc.Add("track_point",[this](std::stringstream &sstream){
                    glm::vec3 point;
                    sstream >> point;
                    m_track_points.push_back(point);
                });
            proc.Process(lines);

            if(!m_track_points.empty())
            {
                m_current_point = m_track_points.begin();
            }
        }

        virtual bool LoadBrain(std::istream &is)
        {
            std::vector<std::string> lines;
            LoaderUtility::LoadLineBlock(is,"never_exists",lines);
            UpdateFromLines(lines);
            return true;
        }

        

        private:
        const int random_maximum = 150;
        int rotator;
        static std::default_random_engine random_generator;
    };

    std::shared_ptr<IBrain> CreateBrain(BrainTypes brain_type, std::function<void(GlCharacter & character)> world_reaction)
    {
        switch(brain_type)
        {
            case BrainTypes::Hero:
                return std::make_shared<BrainHero>(world_reaction);
            case BrainTypes::Npc:
                return std::make_shared<BrainMob>(world_reaction);
            default:
                return std::make_shared<BrainEmpty>();
        }
    }

    std::default_random_engine BrainMob::random_generator;
}