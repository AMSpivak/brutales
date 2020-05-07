#ifndef GL_I_GL_GAME_STATE_MENU
#define GL_I_GL_GAME_STATE_MENU
//#define GLEW_STATIC
//#include <GL/glew.h>
#include "i_gl_game_state.h"
#include "gl_light.h"
#include "gl_render_target.h"
#include "gl_model.h"
#include "gl_character.h"
#include "gl_dungeon.h"
#include "i_map_event.h"
#include "loader.h"
#include "gl_font16x16.h"
#include "gl2d_item.h"
#include "gl2d_image.h"
#include "map_heightmap.h"
#include <list>
#include <sound/irrKlang.h>
#include "gl2d_interface.h"
#include <GL/glfw3.h>

class GlGameStateMenu: public IGlGameState
{

public:
    GlGameStateMenu(std::map<const std::string,GLuint> &shader_map,
        std::map<std::string,std::shared_ptr<glRenderTargetSimple>> &render_target_map,
        std::map<std::string,std::shared_ptr<GlCharacter>> &models_map,
        GLResourcesManager &resources_manager, States &states_map,
        size_t screen_width,
        size_t screen_height,
        irrklang::ISoundEngine *sound_engine,
        GLFWwindow* window
        );

    ~GlGameStateMenu()
    {
        
    }
    void Draw();
    std::weak_ptr<IGlGameState> Process(std::map <int, bool> &inputs, float joy_x, float joy_y);
    void SwitchIn(){}
    void SwitchOut(){}
private:

    bool m_execute;

    irrklang::ISoundEngine  *m_sound_engine;
    bool m_show_intro;
    GLFWwindow* m_window;
    std::weak_ptr<IGlGameState> m_return_state;

    std::shared_ptr<IGlText> m_gl_text;

    Gl2D::Interface2D m_interface;
    std::map<std::string,std::shared_ptr<glRenderTargetSimple>> &m_render_target_map;
    std::map<std::string,std::shared_ptr<GlCharacter>> & m_models_map;
    std::list<std::string> m_messages;

    std::map<std::string,std::vector<std::string>> m_scripts;
    
    std::string m_level_file;

    
    bool m_antialiase_enabled;
    int now_frame;
    double time;
    double m_interface_time;
    LoaderUtility::LinesProcessor m_message_processor;

    void LoadScript(std::vector<std::string> &lines);
    void Draw2D(GLuint depth_map);

    void PostMessage(const std::string & event_string);
    void ProcessMessages();
    void ProcessInputs(std::map <int, bool> &inputs);  
    
};

#endif
