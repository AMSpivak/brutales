//#define GLEW_STATIC
#include <GL/glew.h>
#include <GL/glfw3.h>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <functional>
#include <utility>
#include <tuple>
#include <algorithm>
#include <set>
#include <array>
#include <math.h>  
#include "math3d.h"  
//#define GLM_SWIZZLE_XYZW

#include "glm/glm.hpp"

#include "glm/trigonometric.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "gl_physics.h"
#include "gl_game_state_dungeon.h"


#include "map_event_general.h"
#include "collision.h"
#include "loader.h"
#include "engine_settings.h"
#include "game_status.h"
#include "game_event_fabric.h"
#include "gl2d_progressbar.h"
#include "glresourses.h"
#include "game_inputs.h"
#include "brain.h"
//#include "glfw3.h"

constexpr float sound_mul = 0.1f;



void ResetModels(std::vector <std::shared_ptr<glModel> > &Models)
{
    for(auto &tmpModel : Models)
    {
        //tmpModel->model = glm::rotate(tmpModel->model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        //tmpModel->model = glm::rotate(tmpModel->model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    }
}


bool GlGameStateDungeon::AddObjectsFromFile(const std::string & object)
{
    std::ifstream objects_file;
	objects_file.open(object);
    if(objects_file.is_open())
    {
        std::vector<std::string> obj_lines;

        while(!objects_file.eof())
        {
            std::string prefix = LoaderUtility::FindPrefix(objects_file);
            if(prefix == "object")
            {
                LoaderUtility::LoadLineBlock(objects_file,prefix,obj_lines);
                LoadObject(obj_lines);
            }
        }
        objects_file.close();
        return true;
    }
    else
        return false;
}

bool GlGameStateDungeon::AddObjectFromFile(const std::string & object,const std::string & name,glm::vec3 position)
{
    std::ifstream objects_file;
	objects_file.open(object);
    if(objects_file.is_open())
    {
        std::vector<std::string> obj_lines;

        if(!objects_file.eof())
        {
            LoaderUtility::LoadLineBlock(objects_file,LoaderUtility::FindPrefix(objects_file),obj_lines);
            auto object = LoadObject(obj_lines);
            object->SetPosition(position);
            object->SetName(name);
        }
        objects_file.close();
        return true;
    }
    else
        return false;
}

GlGameStateDungeon::GlGameStateDungeon(std::map<const std::string,GLuint> &shader_map,
                                    std::map<std::string,std::shared_ptr<glRenderTargetSimple>> & render_target_map,
                                    std::map<std::string,std::shared_ptr<GlCharacter>> & models_map,
                                    GLResourcesManager &resources_manager, States &states_map,
                                    size_t screen_width,
                                    size_t screen_height,
                                    irrklang::ISoundEngine *sound_engine):
                                                        IGlGameState(shader_map,resources_manager, states_map,screen_width,screen_height)
                                                        ,return_state()
                                                        ,m_render_target_map(render_target_map)
                                                        ,m_models_map(models_map)
                                                        ,hero(models_map["Hero"])
                                                        ,m_antialiase_enabled(true)
                                                        ,m_start_place("")
                                                        ,light_angle (90.0f)
                                                        ,light_radius (20.0f)
                                                        ,camera_distance(12.f)
                                                        ,camera_height(1.0f)
                                                        ,now_frame(91)
                                                        ,key_angle(0.0f)
                                                        ,camera_rotation_angle(0.0f)
                                                        ,old_joy_x(0.0f)
                                                        ,m_dungeon(10,10,1)
                                                        ,m_show_intro(false)
                                                        ,m_info_message("")
                                                        ,unit_control_action(AnimationCommand::kNone,glm::mat4(1))
                                                        ,simple_screen(0)
                                                        ,m_sound_engine(sound_engine)
                                                        ,m_ready(false)
{
    simple_sky.InitBuffer(512);
    hero->SetBrain(Character::CreateBrain(Character::BrainTypes::Hero,[this](GlCharacter & character){ControlUnit(character);}));

    glClearColor(0.0f,0.0f,0.0f,1.0f);

    float a_ratio = static_cast<float>(screen_width);
    a_ratio /= screen_height;

    auto object_ptr = std::make_shared<Gl2D::GlProgressbar>(-1.0f,0.9f,0.8f,0.1f,a_ratio,
                                GetResourceManager()->m_texture_atlas.Assign("halfbar_border.png"),
                                GetResourceManager()->m_texture_atlas.Assign("halfbar.png"),
                                m_shader_map["sprite2dsimple"],
                                []() { return GameSettings::GetHeroStatus()->GetLife(); }
                                );
                                
    object_ptr->SetItemAligment(Gl2D::ItemAligment::Left);
    object_ptr->SetAspectRatioKeeper(Gl2D::AspectRatioKeeper::Minimal);                    
    Interface2D.push_back(object_ptr);

    m_intro = std::make_shared<Gl2D::GlImage>(-1.0f,-0.56f,2.0f,1.12f,a_ratio,
                                GetResourceManager()->m_texture_atlas.Assign("back.png"),m_shader_map["sprite2dsimple"]);
    m_intro->SetItemAligment(Gl2D::ItemAligment::Center);
    m_intro->SetAspectRatioKeeper(Gl2D::AspectRatioKeeper::Maximal);     

    m_gl_text = std::make_shared<GlText16x16>("font2.png",GetResourceManager()->m_texture_atlas,0.1f,0.1f);

    m_message_processor.Add("teleport",[this](std::stringstream &sstream)
                                    {
                                        std::string level;
                                        std::string start;
                                        sstream >> level >> start;
                                        LoadMap(level,start);
                                    });

    m_message_processor.Add("show_message",[this](std::stringstream &sstream)
                                    {
                                        std::getline(sstream,m_info_message);
                                        //m_info_message = sstream.str();
                                        //std::cout<<m_info_message<<"\n";
                                    });
    
    m_message_processor.Add("pause_interface",[this](std::stringstream &sstream)
                                    {
                                        pause_interface.duration = LoaderUtility::GetFromStream<double>(sstream);
                                        pause_interface.start_time = glfwGetTime();
                                    });

    m_message_processor.Add("spawn",[this](std::stringstream &sstream)
                                    {
                                        std::string object;
                                        std::string name;
                                        glm::vec3 position;
                                        sstream >> object >> name >> position;
                                        AddObjectFromFile(object,name,position);
                                    });

    m_message_processor.Add("set_state", [this](std::stringstream& sstream)
        {
            std::string state;
            sstream >> state;
            std::cout << "set_state " << state <<"\n";
            return_state = m_states_map[state];
        });

    m_message_processor.Add("rotate",[this](std::stringstream &sstream)
                                    {
                                        std::string name;
                                        sstream >> name;
                                        auto object = FindSharedCollectionByName(dungeon_objects.begin(), dungeon_objects.end(),name);
                                        if(object != nullptr)
                                        {
                                            float angle =0.0f;
                                            sstream >>angle;
                                            object->model_matrix = glm::rotate(object->model_matrix, glm::radians(angle), LoaderUtility::GetFromStream<glm::vec3>(sstream));
                                            object->RefreshMatrixes();
                                        }
                                    });

    m_message_processor.Add("run_script",[this](std::stringstream &sstream)
                                    {
                                        std::string name;
                                        sstream >> name;
                                        try
                                        {
                                            const auto &script = m_scripts.at(name);
                                            for(const auto &message: script)
                                            {
                                                PostMessage(message);
                                            }
                                        }
                                        catch(const std::out_of_range& oor)
                                        {
                                            std::cout << "no such script: " << name << "\n";
                                            std::cout << oor.what() <<"\n";
                                        }
                                        
                                    });
    
    m_message_processor.Add("play_ani",[this](std::stringstream &sstream)
                                {
                                    std::string name;
                                    sstream >> name;
                                    auto object = FindSharedCollectionByName(dungeon_objects.begin(), dungeon_objects.end(),name);
                                    if(object != nullptr)
                                    {
                                        object->UseSequence(LoaderUtility::GetFromStream<std::string>(sstream));
                                    }
                                });                         

    m_message_processor.Add("hero_strike",[this](std::stringstream &sstream)
    {                                
        mob_events.push_back(GameEvents::CreateGameEvent(GameEvents::EventTypes::HeroStrike,hero.get()));
    });

    m_message_processor.Add("sound3d",[this](std::stringstream &sstream)
    {  
        std::string name;
        std::string sound;
        sstream >> name >> sound;   
        //m_sound_engine->play2D(sound.c_str(), GL_FALSE);
        auto obj = FindSharedCollectionByName(dungeon_objects.begin(), dungeon_objects.end(),name);
        if(obj)
        {    
            auto vec = sound_mul *obj->GetPosition();
            m_sound_engine->play3D(sound.c_str(),irrklang::vec3df(vec[0],vec[1],vec[2]), false, false, false);
        }
    });

    m_message_processor.Add("strike",[this](std::stringstream &sstream)
    { 
        //m_sound_engine->play2D("material/audio/punch.wav", GL_FALSE);
        std::string name;
        float force = 0.0f;
        sstream >> name >> force;
        auto obj = FindSharedCollectionByName(dungeon_objects.begin(), dungeon_objects.end(),name);//MobPointer(name);
        
        if(obj)
        {
            glRenderTargetDeffered &render_target = *(dynamic_cast<glRenderTargetDeffered*>(m_render_target_map["base_deffered"].get()));
            GameEvents::GeneralEventStrike strike;
            strike.source = obj;
            strike.strike_force = force;
            strike.texture = &(fx_texture_2->m_texture);
            //strike.current_shader = m_shader_map["sprite2d"];
            strike.current_shader = m_shader_map["simple"];
            strike.depthmap = render_target.depthMap;
            map_events.push_back(GameEvents::CreateGameEvent(GameEvents::EventTypes::HeroStrike,&strike));
        }
    });

    m_message_processor.Add("hero_use",[this](std::stringstream &sstream)
    {                                
        mob_events.push_back(GameEvents::CreateGameEvent(GameEvents::EventTypes::HeroUse,&(*hero)));
    });

    m_message_processor.Add("show_intro",[this](std::stringstream &sstream)
    {  
        std::string image;
        sstream >> image;
        m_intro->SetImage(GetResourceManager()->m_texture_atlas.Assign(image)); 
        m_mode = GameStateMode::Intro;
        
    });

    Camera.SetCameraLens(45,(float)screen_width / (float)screen_height,0.1f, 400.0f);
    Camera.SetCameraLocation(glm::vec3(12.0f, 8.485f, -12.0f),glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    time = glfwGetTime();

    //LoadMap("levels/test.lvl","base");

    glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);   
}
void GlGameStateDungeon::SwitchIn() 
{
    return_state = std::weak_ptr<IGlGameState>();
}

void GlGameStateDungeon::SwitchOut() 
{
}

void GlGameStateDungeon::SelectStart(std::vector<std::string> &lines)
{
    size_t y = 0;
    std::string start_name;
    for(auto &line : lines)
    {
        size_t x = 0;
        size_t tile = 0;
        std::stringstream ss(line);
        ss >> start_name >> hero_position;
        hero->SetPosition(hero_position);
        if(!start_name.compare(m_start_place)) 
            return;
    }
}


void GlGameStateDungeon::LoadTiles(std::vector<std::string> &lines)
{
    size_t y = 0;
    for(auto &line : lines)
    {
        size_t x = 0;
        size_t tile = 0;
        std::stringstream ss(line);
        while((x< m_dungeon.Width())&&(!ss.eof()))
        {
            ss >> tile;
            m_dungeon.SetTile(x,y,0,tile);
            ++x;            
        }
        ++y;
        if(y>=m_dungeon.Height())
            return;
    }
}

void GlGameStateDungeon::LoadDungeonObjects(std::vector<std::string> &lines)
{
    size_t y = 0;
    for(auto &line : lines)
    {
        size_t x = 0;
        size_t object = 0;
        std::stringstream ss(line);
        while((x<m_dungeon.Width())&&(!ss.eof()))
        {
            ss >> object;
            m_dungeon.SetObject(x,y,0,object);
            ++x;
        }
        ++y;
        if(y>=m_dungeon.Height())
            return;
    }
}

std::shared_ptr<GlCharacter> GlGameStateDungeon::LoadObject(std::vector<std::string> &lines)
{
        auto object_ptr = std::make_shared<GlCharacter>(CharacterTypes::map_object);
        dungeon_objects.push_back(object_ptr);
        object_ptr->UpdateFromLines(lines);
        object_ptr->model_matrix = glm::rotate(object_ptr->model_matrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        return object_ptr;
}

void GlGameStateDungeon::SetDungeonSize(std::vector<std::string> &lines)
{
    std::stringstream ss(lines[0]);
    size_t width = 3;
    size_t height = 3;
    size_t floors = 1;

    ss >> width>> height>>floors;
    m_dungeon = GlDungeon(width,height,floors);
}

void GlGameStateDungeon::LoadScript(std::vector<std::string> &lines)
{
    std::vector<std::string> clean_lines(lines.begin()+1,lines.end());
    m_scripts.insert(std::make_pair(lines[0],clean_lines));
}

void GlGameStateDungeon::SetHeightmap(std::vector<std::string> &lines)
{
    if(lines.size()<=1) 
        return;

    LoaderUtility::LinesProcessor proc;
    std::string map_string;
    float h_scale =1.0f;
    float m_scale =1.0f;
    glm::vec3 map_size;

    proc.Add("map",[this,&map_string](std::stringstream &sstream){sstream >> map_string;});
    proc.Add("height_scale",[this,&h_scale](std::stringstream &sstream){sstream >> h_scale;});
    proc.Add("map_scale",[this,&m_scale](std::stringstream &sstream){sstream >> m_scale;});
    proc.Add("map_size",[this,&map_size](std::stringstream &sstream){sstream >> map_size;});
    proc.Add("walk_min",[this,&map_size](std::stringstream &sstream){sstream >> map_min;});
    proc.Add("walk_max",[this,&map_size](std::stringstream &sstream){sstream >> map_max;});
    proc.Process(lines);
    std::cout<<"\n\n\nmap"<<map_size<<"\n\n\n";

    m_heightmap.LoadMap(map_string);
    m_heightmap.SetParameters(m_scale,h_scale);
    m_heightmap.SetMapSize(map_size);


}

void GlGameStateDungeon::SetMapLight(std::vector<std::string> &lines)
{
    if(lines.size()<=1) 
        return;
 
    LoaderUtility::LinesProcessor proc;
    proc.Add("light_pos",[this](std::stringstream &sstream){sstream >> light_position;});
    proc.Add("skybox",[this](std::stringstream &sstream)
                                    {
                                        std::string sky;
                                        sstream >> sky;
                                        GLResourcesManager * resources_manager = GetResourceManager();
                                        skybox = resources_manager->m_texture_atlas.Assign(sky);
                                    });
    proc.Add("camera_lens",[this](std::stringstream &sstream)
                                    {
                                        float f_near = 1.f;
                                        float f_far = 35.0f;
                                        float size = 20.0f;
                                        sstream >> size >> f_near >> f_far; 
                                        //Lights.SetCameraLens_Orto(-size, size,-size, size,f_near,f_far);                                         
                                        //Light2.SetCameraLens_Orto(-size*2, size*2,-size*2, size*2,f_near,f_far);                                         
                                    
                                    });

    proc.Add("light_color",[this](std::stringstream &sstream){ sstream >> light_color_vector;});
    proc.Process(lines);

    light_dir_vector = glm::normalize(light_position);
    //Lights.SetCameraLocation(light_position,glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    //Light2.SetCameraLocation(light_position,glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

}


void GlGameStateDungeon::LoadMapEvent(std::vector<std::string> &lines)
{
    if(lines.size()<=1) 
        return;

    
    std::shared_ptr<MapEventGeneral>e_ptr(new MapEventGeneral(m_shader_map["sprite2d"],0,nullptr,1.0f,1.4f));

    float radius = 0;
    
    LoaderUtility::LinesProcessor proc;
    proc.Add("position",[e_ptr](std::stringstream &sstream){sstream >> e_ptr->position;});
    proc.Add("message",[e_ptr](std::stringstream &sstream)
                                    { 
                                        std::string tmp;
                                        std::getline(sstream,tmp);
                                        e_ptr->SetMessage(tmp);
                                    });
    proc.Add("radius",[e_ptr](std::stringstream &sstream){ sstream >> e_ptr->radius;});
    proc.Process(lines);
    hero_events.push_back(e_ptr);
}

void GlGameStateDungeon::SaveObjects(const std::string &filename)
{ 
    if(!dungeon_objects.empty())
    {
        std::string tmp_filename(filename);

        size_t ext_pos = tmp_filename.find_last_of(".") +1;
        std::string extention = tmp_filename.replace(ext_pos,tmp_filename.length()- ext_pos,"sav");
        std::ofstream savefile;
        savefile.open (extention,std::ofstream::out | std::ofstream::trunc);
        for(const auto &object:dungeon_objects)
        {
            if(object->GetType() == CharacterTypes::map_object)
            {
                savefile  << (*object);
                #ifdef DBG
                std::cout<<(*object);
                #endif
            }
        }
        savefile.close();
        // std::cout<<"sav closed\n";
    }
    
}

void GlGameStateDungeon::LoadMap(const std::string &filename,const std::string &start_place)
{  
    m_dungeon_hero_info.hero = hero;

    m_messages.clear();
    m_daytime_in_hours =12.0f;                                        
    
    std::ifstream level_file;
	level_file.open(filename); 
    // std::cout<<"Level:"<<filename<<" "<<(level_file.is_open()?"-opened":"-failed")<<"\n";  
    if(!level_file.is_open()) return;

    
    hero_position = glm::vec3(10.0f,0.0f,10.0f); 
    hero->SetPosition(hero_position); 
    m_start_place = start_place;
    GLResourcesManager * resources_manager = GetResourceManager();
    hero_events.clear();
    mob_events.clear();
    map_events.clear();
    m_scripts.clear();
    
    dungeon_objects.clear();
    //dungeon_objects.push_back(m_models_map["Hero"]);


    
    std::map<std::string,const std::function<void(std::vector<std::string> &lines)>> execute_funcs;
    
    if(filename != m_level_file)
    {
        SaveObjects(m_level_file);
        // std::cout<<"Level: old saves to"<< m_level_file<<"\n";
        m_level_file = filename;

        std::string tmp_filename(filename);
        size_t ext_pos = tmp_filename.find_last_of(".") +1;
        std::string extention = tmp_filename.replace(ext_pos,tmp_filename.length() - ext_pos,"sav");
        if(!AddObjectsFromFile(extention))
        {
            execute_funcs.insert(std::make_pair("object",[this](std::vector<std::string> &lines){LoadObject(lines);}));
        }
    }
    else
    {
            execute_funcs.insert(std::make_pair("object",[this](std::vector<std::string> &lines){LoadObject(lines);}));
    }

    execute_funcs.insert(std::make_pair("sky",[this](std::vector<std::string> &lines){SetMapLight(lines);}));
    execute_funcs.insert(std::make_pair("heightmap",[this](std::vector<std::string> &lines){SetHeightmap(lines);}));
    execute_funcs.insert(std::make_pair("models",[this](std::vector<std::string> &lines)
                                        {
                                            for(auto &line : lines)
                                            {
                                                Models.emplace_back(std::make_shared<glModel>(line));
                                            }
                                            ResetModels(Models);
                                        }));
    execute_funcs.insert(std::make_pair("starts",[this](std::vector<std::string> &lines){SelectStart(lines);}));
    execute_funcs.insert(std::make_pair("dungeon_params",[this](std::vector<std::string> &lines){SetDungeonSize(lines);}));
    execute_funcs.insert(std::make_pair("dungeon_tiles",[this](std::vector<std::string> &lines){LoadTiles(lines);}));
    execute_funcs.insert(std::make_pair("dungeon_objects",[this](std::vector<std::string> &lines){LoadDungeonObjects(lines);}));
    execute_funcs.insert(std::make_pair("hero_event",[this](std::vector<std::string> &lines){LoadMapEvent(lines);}));
    execute_funcs.insert(std::make_pair("script",[this](std::vector<std::string> &lines){LoadScript(lines);}));
    

    



    Models.clear();
    
    
    std::vector<std::string> lines;
    
    std::string sufix ="";
    while(!level_file.eof())
    {
        sufix = LoaderUtility::FindPrefix(level_file);
        //std::cout<<sufix<<"\n";
        LoaderUtility::LoadLineBlock(level_file,sufix,lines);
        try
        {
            execute_funcs.at(sufix)(lines);
        }
        catch(const std::out_of_range& exp)
        {
            std::cout << exp.what();
            // std::cout<<"Unknown prefix: "<<sufix<<"\n";
        }
    }


    //auto x_w = map_max - map_min;
    //auto x_m = ((map_max + map_min)*0.5f);

    dungeon_objects.push_back(hero);
    hero->UseSequence("stance");
    hero->SetDungeonHeroInfo(&m_dungeon_hero_info);
    for(int i = 0; i <50; i++)
    {
        mob = std::make_shared<GlCharacter>(CharacterTypes::mob);
        if(i<40)
        {
            UpdateCharacterFromFile("heroes/hero_orc.chr",*mob);
            constexpr float scale = 0.6f;
            mob->model_matrix = glm::scale(mob->model_matrix, glm::vec3(scale, scale, scale));
        }
        else
        {
            UpdateCharacterFromFile("heroes/hero_orc_br2.chr",*mob);
            constexpr float scale = 0.7f;
            mob->model_matrix = glm::scale(mob->model_matrix,glm::vec3(scale, scale, scale));
        }
        
        mob->SetName("Mob"+std::to_string(i));
        float mob_x = static_cast<float>(std::rand());
        float mob_z = static_cast<float>(std::rand());
        mob_x = mob_x * 360.0f / RAND_MAX - 180.0f;
        mob_z = mob_z * 360.0f / RAND_MAX - 180.0f;
        float angle_in_radians = static_cast<float>(std::rand());
        angle_in_radians = angle_in_radians * 6.0f / RAND_MAX;

        mob->SetPosition(glm::vec3(mob_x,0.0f,mob_z));
        mob->model_matrix = glm::rotate(mob->model_matrix, angle_in_radians, glm::vec3(0.0f, 1.0f, 0.0f)); 
        mob->SetDungeonHeroInfo(&m_dungeon_hero_info);
        mob->SetDungeonListReference(mob);
        mob->UseSequence("stance");
        auto brain = Character::CreateBrain(Character::BrainTypes::Npc,[this](GlCharacter & character){/*ControlUnit(character);*/});
        std::vector<std::string> lines;
        for(int ib = 0; ib <5; ib++)
        {
            mob_x = 80.0f * std::rand() / RAND_MAX - 40.0f;
            mob_z = 80.0f * std::rand() / RAND_MAX - 40.0f;
            std::ostringstream ss;
            ss <<"track_point " <<glm::vec3(mob_x,0.0f,mob_z);
            lines.push_back(ss.str());
        }
        brain->UpdateFromLines(lines);
        mob->SetBrain(brain);
        
        dungeon_objects.push_back(mob);    
    }



    level_file.close(); 

    fx_texture = resources_manager->m_texture_atlas.Assign("valh.png");  
    fx_attacker_texture = resources_manager->m_texture_atlas.Assign("attacker.png");  
    fx_texture_2 = resources_manager->m_texture_atlas.Assign("fireball.png");  
    GetResourceManager()->Clean(); 
    m_info_message = "";
    PostMessage("run_script start_script");

}


void GlGameStateDungeon::DrawDungeon(GLuint &current_shader,std::shared_ptr<GlCharacter>hero,const GlScene::glCamera &camera, bool locked_shader)
{
    GlScene::Scene scene;
    scene.LockedShader = locked_shader;
    scene.render_shader =  current_shader;
    scene.render_camera = &camera;       
    scene.zero_offset = hero_position;

    auto point_ldf = glm::vec3(camera.GetFrustrumPoint(GlScene::FrustrumPoints::FarLD)) + hero_position;
    auto point_run = glm::vec3(camera.GetFrustrumPoint(GlScene::FrustrumPoints::NearRU)) + hero_position;
    
    for(auto &object : dungeon_objects)
    {
        const auto& pos = object->GetPosition();
        auto pos_ldf = pos - point_ldf;
        if(glm::dot(pos_ldf, camera.GetFrustrumNormal(GlScene::FrustrumNormals::Far)) > object->radius * 2.0f)
            continue;
        if (glm::dot(pos_ldf, camera.GetFrustrumNormal(GlScene::FrustrumNormals::Left)) > object->radius * 2.0f)
            continue;
        if (glm::dot(pos_ldf, camera.GetFrustrumNormal(GlScene::FrustrumNormals::Down)) > object->radius * 2.0f)
            continue;


        auto pos_run = pos - point_run;

        if (glm::dot(pos_run, camera.GetFrustrumNormal(GlScene::FrustrumNormals::Near)) > object->radius * 2.0f)
            continue;
        if (glm::dot(pos_run, camera.GetFrustrumNormal(GlScene::FrustrumNormals::Right)) > object->radius * 2.0f)
            continue;
        if (glm::dot(pos_run, camera.GetFrustrumNormal(GlScene::FrustrumNormals::Up)) > object->radius * 2.0f)
            continue;

        object->Draw(scene,glm::translate(glm::mat4(), object->GetPosition() - hero_position));
    }

    for (auto model : scene.model_list) model->Draw(scene, EngineSettings::GetEngineSettings()->GetFrame());
}

void DrawSimpleLight(const glm::vec4 &light_pos_vector,const glm::vec3 &light_color_vector,const glm::vec3 &camera_position,GLuint current_shader,glRenderTargetDeffered &render_target)
{
    glClear(GL_DEPTH_BUFFER_BIT);
    //current_shader = m_shader_map["deffered_simple"];
    glUseProgram(current_shader);

	glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, render_target.AlbedoMap);

	glUniform1i(glGetUniformLocation(current_shader, "NormalMap"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, render_target.NormalMap);

	glUniform1i(glGetUniformLocation(current_shader, "PositionMap"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, render_target.PositionMap);

    GLuint view_pos  = glGetUniformLocation(current_shader, "viewPos");
	glUniform3fv(view_pos, 1, glm::value_ptr(camera_position));


	GLuint light_pos  = glGetUniformLocation(current_shader, "LightLocation");
	glUniform4fv(light_pos, 1, glm::value_ptr(light_pos_vector));

        
    GLuint light_color  = glGetUniformLocation(current_shader, "LightColor");
    glUniform3fv(light_color, 1, glm::value_ptr(light_color_vector));

    renderQuad();
}

void GlGameStateDungeon::DrawLight(const glm::vec4 &light_pos_vector, glRenderTargetDeffered &render_target )
{
        //DrawSimpleLight(light_pos_vector,light_color_vector,current_shader,render_target );

    GLuint current_shader = m_shader_map["deffered_simple"];

    glm::vec4 light_position;
    glm::vec3 light_color;
    
    for(auto &event :map_events) 
    {
        if(event->IsLight(light_position,light_color))
        {
            DrawSimpleLight(light_position - light_pos_vector,light_color,Camera.m_position,current_shader,render_target );
        }
    }

    for(auto &object :dungeon_objects) 
    {
        if(object->IsLight(light_position,light_color))
        {
            DrawSimpleLight(light_position - light_pos_vector,light_color,Camera.m_position,current_shader,render_target );
        }
    }
}

void GlGameStateDungeon::DrawFxSprite(GLuint &current_shader, GLuint texture)
{
    glDisable(GL_CULL_FACE);
    if(current_shader == 0)
    {
        current_shader = m_shader_map["sprite"];
    }
    glUseProgram(current_shader);

    glm::mat4 model_m = glm::mat4(1.0f);
    model_m = glm::translate(model_m,glm::vec3(0.5f,0.5f,0.0f));
    glm::mat4 camera_m = glm::mat4(1.0f);
    GLuint cameraLoc  = glGetUniformLocation(current_shader, "camera");
   //glUniformMatrix4fv(cameraLoc, 1, GL_FALSE, glm::value_ptr(Camera.CameraProjectionMatrix()));
    glUniformMatrix4fv(cameraLoc, 1, GL_FALSE, glm::value_ptr(camera_m));

    GLuint model_matrix  = glGetUniformLocation(current_shader, "model");
    //glUniformMatrix4fv(model_matrix, 1, GL_FALSE, glm::value_ptr(model_m));
    glUniformMatrix4fv(model_matrix, 1, GL_FALSE, glm::value_ptr(Camera.CameraProjectionMatrix()));
  
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    renderQuad();
    glEnable(GL_CULL_FACE);
}


void GlGameStateDungeon::Draw2D(GLuint depth_map)
{
    for(auto &event :map_events) 
    {
        event->Show(hero_position,Camera);
    }

    {
        float w =0.5f;
        auto sh = m_shader_map["sprite2d"];
        glm::vec3 position = glm::vec3(0,4.0,0);
        glm::vec4 color = glm::vec4(1.0f,1.0f,1.0f,1.0f);
        glm::vec4 color1 = glm::vec4(1.0f,0.78f,0.055f,1.0f);
        glm::vec4 color2 = glm::vec4(1.0f,0.0f,0.0f,1.0f);
        float a = 1.0f;

        glEnable(GL_ALPHA_TEST);
        glEnable(GL_BLEND);	
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_CULL_FACE);
        
        // for(auto &p_attacker : m_dungeon_hero_info.attackers)
        // {

        //         float mix = glm::clamp((m_dungeon_hero_info.now_time - p_attacker.first),0.0,1.0);
        //         mix = mix * mix;
        //         color =(color2 * mix + color1 * (1.0f - mix));
        //         color[3] = 1.0f;
        //         auto w_t = w * (mix+ 0.1f);
        //         if(auto attacker = p_attacker.second.lock())
        //         {
        //             renderBillBoardDepth(sh,depth_map,&fx_attacker_texture->m_texture,   
        //             w_t,w_t,color * a,position + attacker->GetPosition(),hero_position,Camera);
        //         }
        // }
        // if(auto arch = hero->arch_enemy.lock())
        // {
        //     float w_t = 0.8f;
        //     position = glm::vec3(0,5.0,0);
        //     renderBillBoardDepth(sh,depth_map,&fx_attacker_texture->m_texture,   
        //     w_t,w_t,color2 * a,position + arch->GetPosition(),hero_position,Camera);
  
        // }

    }
    
    glClear( GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

    glEnable(GL_ALPHA_TEST);
    glEnable(GL_BLEND);	
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
    
    for(auto &item :Interface2D) 
    {
        item->Draw();
    }
    glEnable(GL_CULL_FACE);
	glDisable(GL_ALPHA_TEST);
    glDisable(GL_BLEND);

    const float text_size_y = 0.060f;
    const float text_size_x = m_aspect_ratio * text_size_y;

    m_gl_text->SetTextSize(text_size_x,text_size_y); 
    auto shader = m_shader_map["sprite2dsimple"];
    std::stringstream ss;
    ss<< std::fixed<<std::setprecision(1)<<EngineSettings::GetEngineSettings() ->GetFPS()<<" FPS; life: "<<std::setprecision(2)<<GameSettings::GetHeroStatus()->GetLife();
    m_gl_text->DrawString(ss.str(),1.0f - m_gl_text->GetStringLength(ss.str()),1.0f - text_size_y*1.2f, shader);

    if(m_info_message.length()!=0) 
    {
        m_gl_text->DrawString(m_info_message, - 0.5f * m_gl_text->GetStringLength(m_info_message),-1.0f + text_size_y*2.2f, shader);
    }

}
void GlGameStateDungeon::PrerenderLight(glLight& light, std::shared_ptr<GlCharacter>hero, size_t frustrum_sector)
{
    if (simple_screen)
        return;

    {

        light.SetLigtRender();
        //glDrawBuffer(GL_NONE);
        //glReadBuffer(GL_NONE);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(1.1f* frustrum_sector, 80000.0f);

        glClear(GL_DEPTH_BUFFER_BIT);
        GLuint current_shader = m_shader_map["shadowmap"];
        glUseProgram(current_shader);
        unsigned int cameraLoc = glGetUniformLocation(current_shader, "camera");
        glUniformMatrix4fv(cameraLoc, 1, GL_FALSE, glm::value_ptr(light.CameraMatrix()));
        DrawDungeon(current_shader, hero, light, true);

        m_heightmap.Draw(m_shader_map["simple_heightmap"], hero_position, light, frustrum_sector);

        glDisable(GL_POLYGON_OFFSET_FILL);
        glPolygonMode(GL_FRONT, GL_FILL);

    }
    
}

void GlGameStateDungeon::DrawGlobalLight(const GLuint light_loc, const glLight &light)
{
		glActiveTexture(GL_TEXTURE3);
        //glBindTexture(GL_TEXTURE_2D, light.ExpDepthMap);
        glBindTexture(GL_TEXTURE_2D, light.depthMap);
		glUniformMatrix4fv(light_loc, 1, GL_FALSE, glm::value_ptr(light.CameraMatrix()));

		renderQuad();
}


void GlGameStateDungeon::Draw()
{
    if(!m_ready)
    {
        return;
    }

    hero_position = hero->GetPosition();
    

    glRenderTargetDeffered &render_target = *(dynamic_cast<glRenderTargetDeffered*>(m_render_target_map["base_deffered"].get()));
    glRenderTarget &final_render_target = *(dynamic_cast<glRenderTarget*>(m_render_target_map["final"].get()));
    glRenderTarget &postprocess_render_target = *(dynamic_cast<glRenderTarget*>(m_render_target_map["postprocess"].get()));
    glRenderTargetSimple &buffer1_render_target = *(m_render_target_map["buffer_1"].get());
    glRenderTargetSimple &buffer2_render_target = *(m_render_target_map["buffer_2"].get());

    size_t width = IGlGameState::m_screen_width;
    size_t height = IGlGameState::m_screen_height;
    
    if(m_mode == GameStateMode::Intro)
    {

        
        glDisable(GL_STENCIL_TEST);
        glDisable(GL_DEPTH_TEST);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, width, height);
        GLuint current_shader = m_shader_map["fullscreen"];

        glClearColor(0.0f,0.0f,0.0f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // glUseProgram(current_shader);


        // glActiveTexture(GL_TEXTURE0);
        // glBindTexture(GL_TEXTURE_2D, postprocess_render_target.AlbedoMap);
        // renderQuad();/**/

        m_intro->Draw();
        if(m_info_message.length()!=0) 
        {
            const float text_size_y = 0.050f;
            const float text_size_x = m_aspect_ratio * text_size_y;

            m_gl_text->SetTextSize(text_size_x,text_size_y); 
            auto shader = m_shader_map["sprite2dsimple"];
            m_gl_text->DrawString(m_info_message, - 0.5f * m_gl_text->GetStringLength(m_info_message),-1.0f + text_size_y*2.2f, shader);
        }
        //Draw2D(render_target.depthMap);

        glEnable(GL_DEPTH_TEST);
        return;
    }

    if(processed)
    {
        processed = false;
        int models_count = Models.size();

		glDisable(GL_MULTISAMPLE);
		glDisable(GL_CULL_FACE);

		unsigned int cameraLoc;

        
        for (size_t i = 0; i < Lights.size(); i++)//auto & light : Lights)
        {
            PrerenderLight(Lights[i], hero,i);
        }
        //PrerenderLight(Light2,hero);

		glCullFace(GL_BACK);
		glEnable(GL_CULL_FACE);


		//*------------------------------
		render_target.set();
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glCullFace(GL_BACK);
        glDepthFunc(GL_LEQUAL);


        //m_heightmap.Draw(m_shader_map["simple_heightmap"],hero_position,Camera.CameraMatrix());
  

		GLuint current_shader = m_shader_map["deff_1st_pass"];
		glUseProgram(current_shader);
		cameraLoc  = glGetUniformLocation(current_shader, "camera");
		glUniformMatrix4fv(cameraLoc, 1, GL_FALSE, glm::value_ptr(Camera.CameraMatrix()));
        glPolygonMode( GL_FRONT_AND_BACK, EngineSettings::GetEngineSettings()->IsPbrON()?GL_FILL: GL_LINE );

        DrawDungeon(current_shader,hero,Camera,false);
        m_heightmap.Draw(m_shader_map["deff_1st_pass_heght"],hero_position,Camera);

        
        glPolygonMode( GL_FRONT_AND_BACK,GL_FILL );
        
     
		final_render_target.set();
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);


		//glViewport(0, 0, width, height);



		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		current_shader = m_shader_map["luminocity"];

        glEnable(GL_STENCIL_TEST);
        glClear(GL_STENCIL_BUFFER_BIT); 
        glStencilMask(0xFF);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE); 


		glUseProgram(current_shader);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, render_target.PositionMap);

		renderQuad();

		glClear(GL_DEPTH_BUFFER_BIT);

        //current_shader = m_shader_map[EngineSettings::GetEngineSettings()->IsPbrON()?"deffered_global":"deffered_cheap"];
        current_shader = m_shader_map["deffered_global"];


		glUseProgram(current_shader);

		glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, render_target.AlbedoMap);

		glUniform1i(glGetUniformLocation(current_shader, "NormalMap"), 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, render_target.NormalMap);

		glUniform1i(glGetUniformLocation(current_shader, "PositionMap"), 2);
		glActiveTexture(GL_TEXTURE2);
        //glBindTexture(GL_TEXTURE_2D, render_target.PositionMap); render_target.depthMap
        glBindTexture(GL_TEXTURE_2D, render_target.depthMap);

        glUniform1i(glGetUniformLocation(current_shader, "skybox"), 4);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_CUBE_MAP, simple_sky.AlbedoMap);


		GLuint light_dir  = glGetUniformLocation(current_shader, "LightDir");
		glUniform3fv(light_dir, 1, glm::value_ptr(light_dir_vector));

		GLuint view_pos  = glGetUniformLocation(current_shader, "viewPos");
		glUniform3fv(view_pos, 1, glm::value_ptr(Camera.m_position));

        
        GLuint light_color  = glGetUniformLocation(current_shader, "LightColor");

        float sun_angle =  (m_daytime_in_hours - 12.0f)*360.0f/24.0f;
        sun_angle = glm::cos(glm::radians(sun_angle));

        glm::vec3 red_light(0.6f, 0.05f, 0.0f);
        glm::vec3 yellow_light(0.8f, 0.2f, 0.0f);
        glm::vec3 day_light(1.0f);
        
        float mix_yellow = glm::smoothstep(0.15f, 0.45f, sun_angle);
        float mix_red = glm::smoothstep(0.1f, 0.35f, sun_angle);
        day_light = (yellow_light * (1.0f - mix_yellow) + day_light * mix_yellow);
        actual_light_color_vector = 0.1f*((red_light * (1.0f - mix_red ) + day_light * mix_red) * light_color_vector);// glm::vec3(light_color_vector[0] * r, light_color_vector[1] * g, light_color_vector[2] * b);

        sun_angle = glm::clamp(sun_angle + 0.05f, 0.0f, 1.0f);
        float r = glm::smoothstep(0.0f, 0.2f, sun_angle);

        glm::vec4 tmp_light_color = glm::vec4(actual_light_color_vector,r);

        glUniform4fv(light_color, 1, glm::value_ptr(tmp_light_color));

        //glEnable(GL_STENCIL_TEST);
        //glClear(GL_STENCIL_BUFFER_BIT); 
        glStencilMask(0xFF);
        glStencilFunc(GL_EQUAL, 1, 0xFF);
        glStencilOp(GL_KEEP, GL_KEEP, GL_INCR); 

        GLuint ligh_loc = glGetUniformLocation(current_shader, "lightSpaceMatrix");
        GLuint proj_inv = glGetUniformLocation(current_shader, "ProjInv");
        GLuint view_inv = glGetUniformLocation(current_shader, "ViewInv");
        glUniform1i(glGetUniformLocation(current_shader, "shadowMap"), 3);
        const glm::vec4 light_colors[] = { {1.0f,0.0f,0.0f,1.0f},{0.0f,1.0f,0.0f,1.0f},{0.0f,0.0f,1.0f,1.0f},{1.0f,1.0f,0.0f,1.0f} };

        glm::mat4 proj_inv_mat = glm::inverse(Camera.CameraProjectionMatrix());
        glm::mat4 view_inv_mat = glm::inverse(Camera.CameraViewMatrix());
        
        glUniformMatrix4fv(proj_inv, 1, GL_FALSE, glm::value_ptr(proj_inv_mat));
        glUniformMatrix4fv(view_inv, 1, GL_FALSE, glm::value_ptr(view_inv_mat));

        for (size_t i = 0; i < Lights.size(); i++)//const auto & light : Lights)
        {
            glUniform4fv(light_color, 1, glm::value_ptr(light_colors[i]));
            DrawGlobalLight(ligh_loc, Lights[i]);
        }
        
        glDisable(GL_STENCIL_TEST);    
        DrawLight(glm::vec4(hero_position,0.0f),render_target);

		postprocess_render_target.set();


        glDisable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);

		glClearColor(1.0f, 0.4f, 0.4f, 1.0f);


		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        {// sky
            //prerender
            


            current_shader = m_shader_map["pre_skybox"];
    		glUseProgram(current_shader);
            glm::mat4 model_m = glm::inverse(Camera.CameraMatrix());
            cameraLoc  = glGetUniformLocation(current_shader, "camera");
		    //glUniformMatrix4fv(cameraLoc, 1, GL_FALSE, glm::value_ptr(Camera.CameraMatrix()));

            light_dir  = glGetUniformLocation(current_shader, "LightDir");
		    glUniform3fv(light_dir, 1, glm::value_ptr(light_dir_vector));

       
            GLuint light_color  = glGetUniformLocation(current_shader, "LightColor");
            glUniform3fv(light_color, 1, glm::value_ptr(actual_light_color_vector));
            glm::vec3 skycolor{0.6f,0.6f,0.8f};
            skycolor *= 2.0f;
            GLuint sky_color = glGetUniformLocation(current_shader, "SkyColor");
            glUniform3fv(sky_color, 1, glm::value_ptr(skycolor));

            glActiveTexture(GL_TEXTURE0);
            //glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.get()->m_texture);
            
            simple_sky.set();

            for (unsigned int i = 0; i < 6; ++i)
            {
                auto view = simple_sky.SwitchSide(i);
                glUniformMatrix4fv(cameraLoc, 1, GL_FALSE, glm::value_ptr(*view));

                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                renderQuad();
            }
            //glBindTexture(GL_TEXTURE_CUBE_MAP, simple_sky.AlbedoMap);

            glGenerateTextureMipmap(simple_sky.AlbedoMap);

            //current_shader = m_shader_map["skybox"];
            current_shader = m_shader_map["pre_skybox"];
            glUseProgram(current_shader);
            cameraLoc = glGetUniformLocation(current_shader, "camera");
            
            //glActiveTexture(GL_TEXTURE0);
            //glBindTexture(GL_TEXTURE_CUBE_MAP, simple_sky.AlbedoMap);
            glUniformMatrix4fv(cameraLoc, 1, GL_FALSE, glm::value_ptr(model_m));
            skycolor = {0.0f,0.6f,1.0f};
            glUniform3fv(sky_color, 1, glm::value_ptr(skycolor));
            postprocess_render_target.set();

            
            renderQuad();
            

        }
        glClear(GL_DEPTH_BUFFER_BIT);
        
		current_shader = m_shader_map[/*m_antialiase_enabled?"sobel_aa":*/"sobel_aa"];

		glUseProgram(current_shader);


		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, render_target.AlbedoMap);

		glUniform1i(glGetUniformLocation(current_shader, "LightMap"), 1);
		glActiveTexture(GL_TEXTURE0+1);
		glBindTexture(GL_TEXTURE_2D, final_render_target.AlbedoMap);

        glUniform1i(glGetUniformLocation(current_shader, "SpecMap"), 2);
		glActiveTexture(GL_TEXTURE0+2);
		glBindTexture(GL_TEXTURE_2D, final_render_target.NormalMap);

        glUniform1i(glGetUniformLocation(current_shader, "DepthMap"), 3);
        glActiveTexture(GL_TEXTURE0+3);
        glBindTexture(GL_TEXTURE_2D, render_target.depthMap);

        renderQuad();/**/

		

        glClear(GL_DEPTH_BUFFER_BIT);

    }

    glDisable(GL_STENCIL_TEST);
    glDisable(GL_DEPTH_TEST);

    {
        buffer1_render_target.set();
        GLuint current_shader = m_shader_map["fullblur_h"];
        glUseProgram(current_shader);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, postprocess_render_target.AlbedoMap);
        renderQuad(); 
        buffer2_render_target.set();
        current_shader = m_shader_map["fullblur_w"];
        glUseProgram(current_shader);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, buffer1_render_target.AlbedoMap);
        renderQuad(); 
    }


    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, width, height);
    GLuint current_shader = m_shader_map["sobel_blur"];

    glUseProgram(current_shader);


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, postprocess_render_target.AlbedoMap);
    //glBindTexture(GL_TEXTURE_2D, render_target.depthMap);

    //glBindTexture(GL_TEXTURE_2D, postprocess_render_target.AlbedoMap);
	//glBindTexture(GL_TEXTURE_2D, render_target.NormalMap);

    glUniform1i(glGetUniformLocation(current_shader, "blurMap"), 1);
    glActiveTexture(GL_TEXTURE0+1);
    //glBindTexture(GL_TEXTURE_2D, render_target.depthMap);

    glBindTexture(GL_TEXTURE_2D, buffer2_render_target.AlbedoMap);
	//glBindTexture(GL_TEXTURE_2D, render_target.NormalMap);


    glUniform1i(glGetUniformLocation(current_shader, "DepthMap"), 2);
    glActiveTexture(GL_TEXTURE0+2);
    //glBindTexture(GL_TEXTURE_2D, render_target.depthMap);
    glBindTexture(GL_TEXTURE_2D, render_target.depthMap);

    renderQuad();/**/

    Draw2D(render_target.depthMap);

    glEnable(GL_DEPTH_TEST);


}


std::pair<float,const glm::vec3> GlGameStateDungeon::FitObjectToMap(GlCharacter& object)
{
    glm::vec3 position = object.GetPosition();

    if(object.GetType() == CharacterTypes::mob || object.GetType() == CharacterTypes::hero)
    {
        position[1] = m_heightmap.GetHeight(position[0],position[2]);
    }

    if(object.mass_inv < 0.001)
        return std::make_pair(0.0f,position);
        
    const glm::vec3 edge=glm::vec3(100.0f,100.0f,100.0f);
    glm::vec3 new_position = glm::clamp(position,map_min,map_max);
    
    return std::make_pair(0.0f,new_position);
}

float GlGameStateDungeon::FitObjectToObject(IGlModel& object1,IGlModel& object2)
{
    float mass_summ = object1.mass_inv + object2.mass_inv;

    if(mass_summ < std::numeric_limits<float>::min())
            return 0.0f;

    auto intersection = Physics::Intersection(object1,object2);
    
    if (intersection.first < std::numeric_limits<float>::min()) 
        return 0.0f;

    float pos2_axe = glm::dot(object2.GetPosition() - object1.GetPosition(),intersection.second);
    intersection.second[1] = 0.0f;
    if(pos2_axe < 0.0f)
    {
        intersection.first = -intersection.first;
    }

    float obj1_part = object1.mass_inv/mass_summ;
    float obj2_part = 1.0f - obj1_part;


    MoveObjectAttempt( object2, intersection.second, obj2_part * intersection.first);
    MoveObjectAttempt( object1, intersection.second, -obj1_part * intersection.first);
    
    return intersection.first;
    
}

InteractionResult GlGameStateDungeon::ReactObjectToEvent(std::weak_ptr<GlCharacter> object,IMapEvent& event,std::string &return_value)
{
    if(!object.expired() && event.IsInteractable(object))
    {
        auto ptr = object.lock();
        auto intersection = Physics::Intersection(*ptr,event);
        return intersection.first < std::numeric_limits<float>::min() ? InteractionResult::Nothing : event.Interact(object,return_value);
    }
    return InteractionResult::Nothing;
}

void GlGameStateDungeon::FitObjects(int steps, float accuracy)
{
    float summ = 0.0f;
    //ApplyMapForces()
    for(int i =0; i< steps; i++)
    {
        summ = 0.0f;

        for(auto it_object1 = dungeon_objects.begin();it_object1 != dungeon_objects.end();it_object1++)
        {
            if(!(*it_object1)->ghost)
            {  
                for(auto it_object2 = std::next(it_object1) ;it_object2 != dungeon_objects.end();it_object2++)
                {  
                    if(!(*it_object2)->ghost)
                    {
                        //summ = std::max(summ, FitObjectToObject(**it_object1, **it_object2));
                        summ = std::max( summ,Physics::Collide(**it_object1,**it_object2));
                    }  
                }
            }
        }
        for(auto &object : dungeon_objects)
        {  
            auto res = FitObjectToMap(*object);
            summ =std::max( summ, res.first);
            
            object->SetPosition(res.second);
            
        }
        if(summ < accuracy)
        {
            break; 
        }
    }
}





bool GlGameStateDungeon::MobKilled(std::shared_ptr<GlCharacter> obj)
{
    glRenderTargetDeffered &render_target = *(dynamic_cast<glRenderTargetDeffered*>(m_render_target_map["base_deffered"].get()));

    std::string event_return_string;
    for(auto &event : map_events)
    {
        ReactObjectToEvent(obj,*event.get(),event_return_string);
    }

    float mob_life = obj->GetLifeValue();
    
    if(obj->GetType() != CharacterTypes::hero)
    {
        for(auto &event : mob_events)
        {
            ReactObjectToEvent(obj,*event.get(),event_return_string);
        }
    }

    
    if (obj->GetLifeValue() < 0.0f)
        {
            GameEvents::GeneralEventStruct info = {&(*obj),m_shader_map["sprite2d"],render_target.depthMap,&(fx_texture->m_texture)};
            map_events.push_back(CreateGameEvent( GameEvents::EventTypes::BarrelValhalla,&info));
            return true;
        }
    return false;
}

bool IsKilled (std::shared_ptr<IMapEvent> value) { return value->Process() == EventProcessResult::Kill; }

void GlGameStateDungeon::MapObjectsEventsInteract()
{
    dungeon_objects.remove_if([this](std::shared_ptr<GlCharacter> obj){return MobKilled(obj);});

    mob_events.remove_if(IsKilled);
    map_events.remove_if(IsKilled);
}

void GlGameStateDungeon::PostMessage(const std::string & event_string)
{
    m_messages.push_back(event_string);
}

void GlGameStateDungeon::ProcessMessages()
{
    double l_time = glfwGetTime();
    while (!m_messages.empty()&&!pause_interface.IsPaused(l_time))
    {
        m_message_processor.Process(m_messages.front());
        m_messages.pop_front();
    }
}
bool GlGameStateDungeon::HeroEventsInteract(std::shared_ptr<GlCharacter> hero_ptr)
{
    std::string event_return_string;
    for(auto &event : hero_events)
    {
       if(ReactObjectToEvent(hero_ptr,*event,event_return_string) == InteractionResult::PostMessage)
       {
           //std::cout<<"\n"<<event_return_string<<"\n"<< hero_ptr->GetPosition()<<"\n";
           
           PostMessage(event_return_string);
           return true;
       }
    }
    return false;
}

void GlGameStateDungeon::ControlUnit(GlCharacter & character)
{
    if(unit_control_action.first == AnimationCommand::kNone)
    {
        unit_control_action.first = AnimationCommand::kStance;
    }

    character.UseCommand(unit_control_action.first);

    character.model_matrix = unit_control_action.second;
}


std::weak_ptr<IGlGameState>  GlGameStateDungeon::Process(std::map <int, bool> &inputs, float joy_x, float joy_y)
{
    if(!m_ready)
    {
        LoadMap("levels/test.lvl","base");
        m_ready = true;
    }

    glRenderTargetDeffered &render_target = *(dynamic_cast<glRenderTargetDeffered*>(m_render_target_map["base_deffered"].get()));

    int models_count = Models.size();
    double time_now = glfwGetTime();
    double passed_time = time_now - time;
    m_dungeon_hero_info.now_time = time_now;

    if(m_mode == GameStateMode::Intro)
    {
        bool press = inputs[GLFW_KEY_SPACE];
        GLFWgamepadstate state;
        if (glfwJoystickIsGamepad(GLFW_JOYSTICK_1)&&glfwGetGamepadState(GLFW_JOYSTICK_1, &state))
        {
            press |= static_cast<bool>(state.buttons[GLFW_GAMEPAD_BUTTON_A]);
        }
        
        if((!pause_interface.IsPaused(time_now)) &&press)
        {
            m_mode = GameStateMode::General;
        }
        return std::weak_ptr<IGlGameState>();
    }
    else
    if((passed_time)>(1.0/30.0))
    {
        //simple_screen = !simple_screen;


        m_daytime_in_hours += 0.00055f;
        if(m_daytime_in_hours>24.0f)
        {
            m_daytime_in_hours -= 24.0f;
        }

        time = time_now;        
        processed = true;
        MapObjectsEventsInteract();
        hero_position = hero->GetPosition();
        HeroEventsInteract(hero);

        ProcessMessages();
        ProcessInputsCamera(inputs,joy_x, joy_y);
              
        unit_control_action = ProcessInputs(inputs);
        
        //ControlUnit(*hero);

        for(auto &object : dungeon_objects)
        {  
            object->Process(m_messages);
        }

        

        m_dungeon_hero_info.attackers.remove_if([](decltype (m_dungeon_hero_info.attackers)::value_type val)
            {   const double attacker_time_limit = 20.0;  
                return val.second.expired() || (val.first > attacker_time_limit); });

        for (auto &attacker : m_dungeon_hero_info.attackers)
        {
            attacker.first += passed_time;
        }

        //{return val.second.expired() || ;});
        m_dungeon_hero_info.attackers.sort([](const decltype (m_dungeon_hero_info.attackers)::value_type& a,
            const decltype (m_dungeon_hero_info.attackers)::value_type& b)
            {return a.first < b.first; });
        std::cout<<"attackers: "<<m_dungeon_hero_info.attackers.size()<<"\n";

        FitObjects(10,0.01f);
        GameSettings::GetHeroStatus()->SetLife(hero->GetLifeValue());
        if(GameSettings::GetHeroStatus()->GetLife() < 0)
        {
            PostMessage("run_script loose_script");
            hero->SetLifeValue(1.0f);
            //return m_states_map["main_menu"];
        }
    }

    return return_state;
}

std::pair<AnimationCommand,const glm::mat4>  GlGameStateDungeon::ProcessInputs(std::map <int, bool> &inputs)
{
    

    if(inputs[GLFW_KEY_F9])
    {
        m_daytime_in_hours -= 0.1f;
    }

    if(inputs[GLFW_KEY_F10])
    {
        m_daytime_in_hours += 0.1f;
    }

    auto move_inputs = GameInputs::ProcessInputsMoveControl(inputs);
    float move_square = move_inputs.first * move_inputs.first + move_inputs.second * move_inputs.second;
    bool moving = move_square > 0.03f;

    glm::mat4 rm(hero->model_matrix);  

    if(inputs[GLFW_KEY_ESCAPE])
    {
        return_state = m_states_map["main_menu"];
        return std::make_pair(AnimationCommand::kNone, rm);
    }
    
    float disorientation = 0;

    glm::vec3 hero_direction;
    glm::vec3 hero_side;
    std::tie(hero_direction, hero_side) = hero->Get2DBasis();


    if(moving)
    {           
        glm::mat3 m = glm::rotate(glm::radians(camera_rotation_angle), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::vec3 new_x = glm::normalize(m * glm::normalize(glm::vec3(move_inputs.first,0.0f,move_inputs.second)));
        disorientation = Math3D::Disorientation(hero_side,new_x, hero_direction);
    }

    auto direction = Math3D::SimplifyDirection(disorientation);

    //std::cout << "\n disorientation "<<disorientation<<" "<<direction<<"\n";
    bool action_use = inputs[GLFW_KEY_LEFT_ALT];
    bool attack = inputs[GLFW_MOUSE_BUTTON_LEFT]||inputs[GLFW_KEY_SPACE];  
    bool fast_move = true;
    bool guard = inputs[GLFW_MOUSE_BUTTON_RIGHT]||inputs[GLFW_KEY_LEFT_CONTROL];

    GLFWgamepadstate state;
    if (glfwJoystickIsGamepad(GLFW_JOYSTICK_1)&&glfwGetGamepadState(GLFW_JOYSTICK_1, &state))
    {
        attack |= static_cast<bool>(state.buttons[GLFW_GAMEPAD_BUTTON_A]);
        action_use |=static_cast<bool>(state.buttons[GLFW_GAMEPAD_BUTTON_X]);
        guard |= static_cast<bool>(state.buttons[GLFW_GAMEPAD_BUTTON_B]);

        if(move_square < 0.36f)
        {
            fast_move = false;
        }
    }

    float enemy_distance = 0.f;
    auto enemy_direction = Math3D::SimpleDirections::Forward;
    auto reaction = hero->GetDamageReaction();

    float dis = 0.0f;
    float min_disorient = 0.7;
    //float enemy_disorient = 0.0f
    if (!hero->IsNoRotateable())
    {
        if (auto enemy = hero->arch_enemy.lock())
        {
            glm::vec3 enemy_vector = enemy->GetPosition() - hero->GetPosition();
            enemy_distance = glm::length(enemy_vector);
            auto target_dir = glm::normalize(enemy_vector);
            float fit = 45.0f;

            if (reaction == DamageReaction::Block || reaction == DamageReaction::StrikeBack)
            {
                fit = 75.0f;
            }
            float enemy_disorient = -Math3D::Disorientation(hero_direction, target_dir, hero_side);
            enemy_direction = Math3D::SimplifyDirection(enemy_disorient);
            rm = glm::rotate(glm::radians(fit * enemy_disorient), glm::vec3(0.0f, 1.0f, 0.0f)) * hero->model_matrix;
        }
        else
        {
            //glm::vec3 direction = hero_direction;
            for (const auto &mob : dungeon_objects)
                if (mob->GetType() == CharacterTypes::mob)
                {
                    const float attack_length = 4.0f;
                    auto enemy_direction = mob->GetPosition() - hero->GetPosition();
                    float l = glm::length(enemy_direction);
                    if (l < attack_length)
                    {
                        auto normed_dir = glm::normalize(enemy_direction);
                        float enemy_disorient = -Math3D::Disorientation(hero_direction, normed_dir, hero_side);
                        float a_disorient = abs(enemy_disorient);
                        if (a_disorient < min_disorient)
                        {
                            min_disorient = a_disorient;
                            dis = enemy_disorient;
                        }
                    }
                }
        }

        if(moving && (reaction != DamageReaction::Block)&& (reaction != DamageReaction::StrikeBack))
        {           
                constexpr float fit = -30.0f;
                rm = glm::rotate(glm::radians(-fit * disorientation), glm::vec3(0.0f, 1.0f, 0.0f)) * hero->model_matrix;
        }
    }

 
    if (min_disorient < 0.6f)
    {
        std::cout << "\n disorientation " << dis << " " << direction << "\n";
    }
    
    if(attack)
    {
        if(!fast_move)
        {
            direction = enemy_direction;
        }

        switch(direction)
        {  
            //case Math3D::SimpleDirections::Forward:
                //return std::make_pair(AnimationCommand::kStrikeForward,rm); 
            break;
            case Math3D::SimpleDirections::Left:
                return std::make_pair(AnimationCommand::kStrikeLeft,hero->model_matrix); 
            break;
            case Math3D::SimpleDirections::Right:
                return std::make_pair(AnimationCommand::kStrikeRight,hero->model_matrix); 
            break;
            default:
            break;
        }

        if(reaction == DamageReaction::Block || reaction == DamageReaction::StrikeBack)
        {
            return std::make_pair(AnimationCommand::kStrike,rm);
        }

        if(min_disorient < 0.9f)
        {

            float fit = 100.0f;
            rm = glm::rotate(glm::radians(fit * dis), glm::vec3(0.0f, 1.0f, 0.0f)) * hero->model_matrix;
        }
        return std::make_pair( enemy_distance > 5.0f ? AnimationCommand::kStrikeForward : AnimationCommand::kStrikeLong,rm);
    }

    if(action_use) 
        return std::make_pair(AnimationCommand::kUse,rm);


    if(guard)
    {
        if(moving && fast_move)
        {
            switch(direction)
            {
                case Math3D::SimpleDirections::Back:
                    return std::make_pair(AnimationCommand::kStepBack,(hero->model_matrix));
                break;
                case Math3D::SimpleDirections::Right:
                    return std::make_pair(AnimationCommand::kStepRight,(hero->model_matrix));
                break;
                case Math3D::SimpleDirections::Left:
                    return std::make_pair(AnimationCommand::kStepLeft,(hero->model_matrix));
                break;
                case Math3D::SimpleDirections::Forward:
                    return std::make_pair(AnimationCommand::kStepForward,(hero->model_matrix));
                break;
                default:
                break;
            }
        }
        return std::make_pair(AnimationCommand::kGuard,rm);
    }

    if(moving)
    {
        if(fast_move&&(direction == Math3D::SimpleDirections::Back))
        {
            return std::make_pair(AnimationCommand::kRotate,(hero->model_matrix));
        }
        return std::make_pair(fast_move ? AnimationCommand::kFastMove:AnimationCommand::kMove,rm);
    }
    return std::make_pair(AnimationCommand::kNone,rm);    
}

void GlGameStateDungeon::ProcessInputsCamera(std::map <int, bool> &inputs,float joy_x, float joy_y)
{
    
        if(inputs[GLFW_KEY_RIGHT_BRACKET]) camera_distance +=0.1f;
        if(inputs[GLFW_KEY_LEFT_BRACKET]) camera_distance -=0.1f;
    
        camera_distance = glm::clamp(camera_distance,3.0f,20.0f);

        float joy_diff = joy_x - old_joy_x;
        if(std::abs(joy_diff) <  0.01f)
        {
            joy_diff = 0.0f;
        }
        old_joy_x = joy_x;

        float joy_diff_y = joy_y - old_joy_y;
        if(std::abs(joy_diff_y) <  0.01f)
        {
            joy_diff_y = 0.0f;
        }
        old_joy_y = joy_y;
        
        GLFWgamepadstate state;
        if (glfwJoystickIsGamepad(GLFW_JOYSTICK_1)&&glfwGetGamepadState(GLFW_JOYSTICK_1, &state))
        {
            joy_diff = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X];
            joy_diff_y = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y];
            // Use as gamepad
        }

        
        const float joy_x_sensetivity = 12.0f;
        camera_rotation_angle = FitRing(camera_rotation_angle - joy_diff * joy_x_sensetivity,0.0f,360.0f);
        const float joy_y_sensetivity = 0.07f;
        camera_height = glm::clamp(camera_height + joy_y_sensetivity * joy_diff_y,-1.0f,1.0f);

        glm::vec3 camera_position = glm::vec3(-camera_distance * glm::cos(glm::radians(camera_rotation_angle)), camera_distance * camera_height,  camera_distance * glm::sin(glm::radians(camera_rotation_angle)));
        float height = 1.0f + m_heightmap.GetHeight(hero_position[0] + camera_position[0],hero_position[2]+camera_position[2]) - hero_position[1];
        if(camera_position[1]<height)
        {
            camera_position[1] = height;
        }


        Camera.SetCameraLocation(camera_position,glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        auto norm = glm::normalize(-camera_position);
        auto sound_pos = sound_mul * (hero_position + camera_position);
        m_sound_engine->setListenerPosition(irrklang::vec3df(sound_pos[0],sound_pos[1],sound_pos[2]),
                                            irrklang::vec3df(norm[0],norm[1],norm[2]));
        
        glm::vec3 light_orientation = glm::normalize(glm::vec3(-camera_position.x,0.0f,-camera_position.z));
        
        constexpr float light_distance = 20.0f;
        constexpr float light_offset = 2.0f;

        float sun_angle =  (m_daytime_in_hours - 12.0f)*360.0f/24.0f;
        
        
        if(!simple_screen)
        {
            light_position = glm::vec3(light_distance * glm::sin(glm::radians(sun_angle)), light_distance * glm::cos(glm::radians(sun_angle)), light_offset);
            light_dir_vector = glm::normalize(light_position);



            const std::array<GlScene::FrustrumPoints, static_cast<size_t>(4)> cam_point_indexes { 
                GlScene::FrustrumPoints::NearLD,
                GlScene::FrustrumPoints::NearLU,
                GlScene::FrustrumPoints::NearRD,
                GlScene::FrustrumPoints::NearRU
            };

            const std::array<GlScene::FrustrumPoints, static_cast<size_t>(4)> cam_point_indexes_end{
                GlScene::FrustrumPoints::FarLD,
                GlScene::FrustrumPoints::FarLU,
                GlScene::FrustrumPoints::FarRD,
                GlScene::FrustrumPoints::FarRU
            };

            auto view = glm::lookAt(light_position, glm::vec3(0.0f, 0.0f, 0.0f), light_orientation);

            

            std::array < glm::vec4, (shadow_cascades + 1) * 4>points;
            //auto iter_points = points.begin();
            auto index = 0;
            for(auto point_index : cam_point_indexes)
            {
                points[index++] = (Camera.GetFrustrumPoint(point_index));
            }
            const int far_offset = shadow_cascades * 4;

            index = far_offset;
            for (auto point_index : cam_point_indexes_end)
            {
                points[index++] = (Camera.GetFrustrumPoint(point_index));
            }

            const std::array<float, static_cast<size_t>(shadow_cascades - 1)> cascade_range{ 0.05f, 0.2f, 0.5f};
            for (int i_split = 1; i_split < shadow_cascades; i_split++)
            {
                auto i_split_offset = i_split * 4;
                for (int i = 0; i < 4; i++)
                {
                    auto range = cascade_range[i_split - 1];
                    points[i_split_offset + i] = (range * points[i + far_offset] + (1.0f - range) * points[i]);
                }
            }

            for(auto &point : points)
            {
                point = view * (point - glm::vec4(light_position, 0));
            }
            

            for(size_t i_split = 0; i_split < shadow_cascades; i_split++)
            {

                float min_x = 1000;
                float max_x = -1000;
                float min_y = 1000;
                float max_y = -1000;
                float min_z = 3000;
                float max_z = 30;


                
                auto split_offset1 = (i_split + 1) * 4;
                auto split_offset2 = (i_split) * 4;
                for (int i = 0; i < 4; i++)
                {
                    float v = points[split_offset1 + i].x;
                    min_x = std::min(min_x, v);
                    max_x = std::max(max_x, v);
                    v = points[split_offset1 + i].y;
                    min_y = std::min(min_y, v);
                    max_y = std::max(max_y, v);
                    v = points[split_offset1 + i].z;
                    min_z = std::min(min_z, v);

                    v = points[split_offset2 + i].x;
                    min_x = std::min(min_x, v);
                    max_x = std::max(max_x, v);
                    v = points[split_offset2 + i].y;
                    min_y = std::min(min_y, v);
                    max_y = std::max(max_y, v);
                    v = points[split_offset2 + i].z;
                    min_z = std::min(min_z, v);
                    max_z = std::max(max_z, v);
                }

                Lights[i_split].SetCameraLocation(light_position, glm::vec3(0.0f, 0.0f, 0.0f), light_orientation);
                Lights[i_split].SetCameraLens_Orto(min_x, max_x, min_y, max_y, -max_z, -min_z);
            }

        }
        else
        {
            /* code */
        }
        
        //Light2.SetCameraLocation(light_position+light_orientation*10.0f,light_orientation*10.0f, light_orientation);    
}

std::shared_ptr<GlCharacter> GlGameStateDungeon::MobPointer(const std::string & name)
{
    auto result = std::find_if(dungeon_objects.begin(),dungeon_objects.end(),[&](std::shared_ptr<GlCharacter> obj){return obj->GetName() == name;});
    if(result == dungeon_objects.end())
    {
        return nullptr;
    }
    else
    {
        return *result;
    }
}


