
#include <GL/glew.h>
#include <GL/glfw3.h>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <functional>
#include <utility>
#include <algorithm>
#include <math.h>
#include <map>

#include "glm/glm.hpp"

#include "glm/trigonometric.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include "gl_physics.h"
#include "gl_game_state_menu.h"


#include "map_event_general.h"
#include "collision.h"
#include "loader.h"
#include "engine_settings.h"
#include "game_status.h"
#include "game_event_fabric.h"

#include "gl2d_progressbar.h"
#include "gl2d_image.h"
#include "gl2d_button.h"

#include "glresourses.h"
#include "game_inputs.h"
#include "brain.h"
//#include "glfw3.h"

constexpr float sound_mul = 0.1f;





// bool GlGameStateMenu::AddObjectsFromFile(const std::string & object)
// {
//     std::ifstream objects_file;
// 	objects_file.open(object);
//     if(objects_file.is_open())
//     {
//         std::vector<std::string> obj_lines;

//         while(!objects_file.eof())
//         {
//             std::string prefix = LoaderUtility::FindPrefix(objects_file);
//             if(prefix == "object")
//             {
//                 LoaderUtility::LoadLineBlock(objects_file,prefix,obj_lines);
//                 LoadObject(obj_lines);
//             }
//         }
//         objects_file.close();
//         return true;
//     }
//     else
//         return false;
// }

// bool GlGameStateMenu::AddObjectFromFile(const std::string & object,const std::string & name,glm::vec3 position)
// {
//     std::ifstream objects_file;
// 	objects_file.open(object);
//     if(objects_file.is_open())
//     {
//         std::vector<std::string> obj_lines;

//         if(!objects_file.eof())
//         {
//             LoaderUtility::LoadLineBlock(objects_file,LoaderUtility::FindPrefix(objects_file),obj_lines);
//             auto object = LoadObject(obj_lines);
//             object->SetPosition(position);
//             object->SetName(name);
//         }
//         objects_file.close();
//         return true;
//     }
//     else
//         return false;
// }

GlGameStateMenu::GlGameStateMenu(std::map<const std::string,GLuint> &shader_map,
                                    std::map<std::string,std::shared_ptr<glRenderTargetSimple>> & render_target_map,
                                    std::map<std::string,std::shared_ptr<GlCharacter>> & models_map,
                                    GLResourcesManager &resources_manager, States &states_map,
                                    size_t screen_width,
                                    size_t screen_height,
                                    irrklang::ISoundEngine *sound_engine, GLFWwindow* window):
                                                        IGlGameState(shader_map,resources_manager,states_map,screen_width,screen_height)
                                                        ,m_render_target_map(render_target_map)
                                                        ,m_models_map(models_map)
                                                        ,m_antialiase_enabled(true)
                                                        ,now_frame(91)
                                                        ,m_sound_engine(sound_engine)
                                                        ,m_execute(true)
                                                        ,m_window(window)
{
    //m_sound_engine->play2D("material/audio/breakout.mp3", GL_TRUE);

    glClearColor(0.0f,0.0f,0.0f,1.0f);


    float a_ratio = static_cast<float>(screen_width);
    a_ratio /= screen_height;
    m_gl_text = std::make_shared<GlText16x16>("font2.png",GetResourceManager()->m_texture_atlas,0.1f,0.1f);
    
    {
        auto object_ptr = std::make_shared<Gl2D::GlImage>(-1.0f,-1.0f,2.0f,2.0f,a_ratio,
                                    GetResourceManager()->m_texture_atlas.Assign("menu.png"),
                                    m_shader_map["sprite2dsimple"]);
                                    
        object_ptr->SetAspectRatioKeeper(Gl2D::AspectRatioKeeper::Minimal);
        m_interface.Add("wall",object_ptr);

        
            /*auto button_ptr1 = std::make_shared<Gl2D::GlButton>(-0.6f,-0.65f,1.2f,0.3f,a_ratio,
                                    GetResourceManager()->m_texture_atlas.Assign("button.png"),GetResourceManager()->m_texture_atlas.Assign("button_p.png"),
                                    m_gl_text,"EXIT",
                                    GetResourceManager()->GetShader("sprite2dsimple"),
                                    [this]{glfwSetWindowShouldClose(m_window, GL_TRUE);});*/
            std::vector<std::string> lines;
            lines.push_back("geometry -0.6 -0.65 1.2 0.3");
            lines.push_back("image button.png");
            lines.push_back("image_active button_p.png");
            lines.push_back("shader sprite2dsimple");
            lines.push_back("text EXIT");

            auto button_ptr1 = std::make_shared<Gl2D::GlButton>(a_ratio);
            button_ptr1->Load(lines);
            button_ptr1->SetAction([this] {glfwSetWindowShouldClose(m_window, GL_TRUE);});
            button_ptr1->SetFont(m_gl_text);

            auto button_ptr2 = std::make_shared<Gl2D::GlButton>(-0.6f,-0.35f,1.2f,0.3f,a_ratio,
                                    GetResourceManager()->m_texture_atlas.Assign("button.png"),GetResourceManager()->m_texture_atlas.Assign("button_p.png"),
                                    m_gl_text,"SETTINGS",
                                    m_shader_map["sprite2dsimple"],[]{});
            auto button_ptr3 = std::make_shared<Gl2D::GlButton>(-0.6f,-0.05f,1.2f,0.3f,a_ratio,
                        GetResourceManager()->m_texture_atlas.Assign("button.png"),GetResourceManager()->m_texture_atlas.Assign("button_p.png"),
                        m_gl_text,"START",
                        m_shader_map["sprite2dsimple"],[this]{m_return_state = m_states_map["main_game"];});
                        
            button_ptr1->AddTab(Inputs::InputCommands::Up,button_ptr2);
            button_ptr1->SetActiveSizer(1.05f);
            
            button_ptr2->AddTab(Inputs::InputCommands::Up,button_ptr3);
            button_ptr2->AddTab(Inputs::InputCommands::Down,button_ptr1);
            button_ptr2->SetActiveSizer(1.05f);

            button_ptr3->AddTab(Inputs::InputCommands::Down,button_ptr2);
            button_ptr3->SetActiveSizer(1.05f);

            m_interface.Add("btn1",button_ptr1);
            m_interface.Add("btn2",button_ptr2);
            m_interface.Add("btn3",button_ptr3);
            m_interface.SetActive("btn3");

            m_interface.GetElement("btn1")->SetParent(m_interface.GetElement("wall"));
            m_interface.GetElement("btn2")->SetParent(m_interface.GetElement("wall"));
            m_interface.GetElement("btn3")->SetParent(m_interface.GetElement("wall"));

    }


    m_message_processor.Add("run_script",[this](std::stringstream &sstream)
                                    {
                                        std::string name;
                                        sstream >> name;
                                        auto script = m_scripts.at(name);
                                        for(auto message:script)
                                        {
                                            PostMessage(message);
                                        }
                                    });

    m_message_processor.Add("sound",[this](std::stringstream &sstream)
    {  
        std::string sound;
        sstream >>  sound;   
        m_sound_engine->play2D(sound.c_str(),false);
    });

    m_message_processor.Add("close", [this](std::stringstream& sstream)
        {
            glfwSetWindowShouldClose(m_window, GL_TRUE);
        });

    m_message_processor.Add("change_state", [this](std::stringstream& sstream)
        {
            std::string state;
            sstream >> state;
            m_return_state = m_states_map["main_game"];
        });

    time = glfwGetTime();
    m_interface_time = time;
    //LoadMap("levels/test.lvl","base");

    glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);   
}


void GlGameStateMenu::LoadScript(std::vector<std::string> &lines)
{
    std::vector<std::string> clean_lines(lines.begin()+1,lines.end());
    m_scripts.insert(std::make_pair(lines[0],clean_lines));
}




void GlGameStateMenu::Draw2D(GLuint depth_map)
{   
    glClearColor(0.0f,0.0f,0.0f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_ALPHA_TEST);
    glEnable(GL_BLEND);	
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
    
    m_interface.Draw();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
	glDisable(GL_ALPHA_TEST);
    glDisable(GL_BLEND);
}


void GlGameStateMenu::Draw()
{
    glRenderTargetDeffered &render_target = *(dynamic_cast<glRenderTargetDeffered*>(m_render_target_map["base_deffered"].get()));
    glRenderTarget &final_render_target = *(dynamic_cast<glRenderTarget*>(m_render_target_map["final"].get()));
    glRenderTarget &postprocess_render_target = *(dynamic_cast<glRenderTarget*>(m_render_target_map["postprocess"].get()));
    glRenderTargetSimple &buffer1_render_target = *(m_render_target_map["buffer_1"].get());
    glRenderTargetSimple &buffer2_render_target = *(m_render_target_map["buffer_2"].get());

    size_t width = IGlGameState::m_screen_width;
    size_t height = IGlGameState::m_screen_height;
    



    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, width, height);

    Draw2D(render_target.depthMap);

    glEnable(GL_DEPTH_TEST);
}

void GlGameStateMenu::PostMessage(const std::string & event_string)
{
    m_messages.push_back(event_string);
}

void GlGameStateMenu::ProcessMessages()
{
    double l_time = glfwGetTime();
    while (!m_messages.empty())//&&!pause_interface.IsPaused(l_time))
    {
        m_message_processor.Process(m_messages.front());
        m_messages.pop_front();
    }
}

std::weak_ptr<IGlGameState>  GlGameStateMenu::Process(std::map <int, bool> &inputs, float joy_x, float joy_y)
{
    glRenderTargetDeffered &render_target = *(dynamic_cast<glRenderTargetDeffered*>(m_render_target_map["base_deffered"].get()));
    //std::shared_ptr<GlCharacter> hero_ptr = m_models_map["Hero"];

    double time_now = glfwGetTime();

    if((time_now - time)>(1.0/30.0))
    {
        time = time_now;        
        ProcessMessages();
    }

    if((time_now - m_interface_time)>(1.0/10.0))
    {
        m_interface_time = time_now;                     
        ProcessInputs(inputs);
    }

    auto return_state = m_return_state;
    m_return_state = std::weak_ptr<IGlGameState>();
    return return_state;
}

void  GlGameStateMenu::ProcessInputs(std::map <int, bool> &inputs)
{
    Inputs::InputCommands input_command = Inputs::InputCommands::None;
    if(inputs[GLFW_KEY_F9])
    {
        //m_daytime_in_hours -= 0.1;
    }

    if(inputs[GLFW_KEY_F10])
    {
        //m_daytime_in_hours += 0.1;
    }

    auto move_inputs = GameInputs::ProcessInputsMoveControl(inputs);
    float move_square = move_inputs.first * move_inputs.first + move_inputs.second * move_inputs.second;
    bool moving = move_square > 0.03f;//(std::abs(move_inputs.first)+std::abs(move_inputs.second)>0.2f);

    if(move_inputs.first>0.3f)
    {
        input_command = Inputs::InputCommands::Right;
    }
    if(move_inputs.first<-0.3f)
    {
        input_command = Inputs::InputCommands::Left;
    }
    if(move_inputs.second>0.3f)
    {
        input_command = Inputs::InputCommands::Up;
    }
    if(move_inputs.second<-0.3f)
    {
        input_command = Inputs::InputCommands::Down;
    }

    bool action_use = inputs[GLFW_KEY_LEFT_ALT];
    bool attack = inputs[GLFW_MOUSE_BUTTON_LEFT]|inputs[GLFW_KEY_SPACE];  
    bool fast_move = inputs[GLFW_KEY_LEFT_SHIFT];

    GLFWgamepadstate state;
    if (glfwJoystickIsGamepad(GLFW_JOYSTICK_1)&&glfwGetGamepadState(GLFW_JOYSTICK_1, &state))
    {
        attack |= static_cast<bool>(state.buttons[GLFW_GAMEPAD_BUTTON_A]);
        action_use |= static_cast<bool>(state.buttons[GLFW_GAMEPAD_BUTTON_X]);
        fast_move |= static_cast<bool>(state.buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER]);
    }

    if(attack)
    {
        input_command = Inputs::InputCommands::Strike;
    }

    m_interface.ProcessInput(input_command);
}



