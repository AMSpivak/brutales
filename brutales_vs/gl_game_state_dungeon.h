#ifndef GL_I_GL_GAME_STATE_DUNGEON
#define GL_I_GL_GAME_STATE_DUNGEON
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

class GlGameStateDungeon: public IGlGameState
{
    struct PauseStruct
    {
        double duration;
        double start_time;
        PauseStruct():duration(0.0),start_time(0.0){}
        bool IsPaused(double time)
        {
            return (time - start_time) < duration;
        }
    };


public:
    GlGameStateDungeon(std::map<const std::string,GLuint> &shader_map,
        std::map<std::string,std::shared_ptr<glRenderTargetSimple>> &render_target_map,
        std::map<std::string,std::shared_ptr<GlCharacter>> &models_map,
        GLResourcesManager &resources_manager, States &states_map,
        size_t screen_width,
        size_t screen_height,
        irrklang::ISoundEngine *sound_engine);

    ~GlGameStateDungeon()
    {
        glDisable(GL_FRAMEBUFFER_SRGB);
    }
    void Draw();
    std::weak_ptr<IGlGameState> Process(std::map <int, bool> &inputs, float joy_x, float joy_y);
    void SwitchIn();
    void SwitchOut();
private:

    bool m_ready;
    std::weak_ptr<IGlGameState> return_state;

    irrklang::ISoundEngine  *m_sound_engine;
    float raged_mode = 0.0f;
    unsigned char simple_screen;

    DungeonHeroInfo m_dungeon_hero_info;

    //std::shared_ptr<IGlTextureStruct> heightmap_texture;
    GameMap::HeightMap m_heightmap;
    glm::vec3 map_max;
    glm::vec3 map_min;

    bool m_show_intro;

    PauseStruct pause_interface;
    std::shared_ptr<IGlText> m_gl_text;

    std::list<std::shared_ptr<Gl2D::Gl2dItem>> Interface2D;


    std::vector <std::shared_ptr<glModel> > Models;
    std::map<std::string,std::shared_ptr<glRenderTargetSimple>> &m_render_target_map;
    std::map<std::string,std::shared_ptr<GlCharacter>> & m_models_map;
    std::shared_ptr<IGlTextureStruct> fx_texture,fx_attacker_texture, fx_texture_2,skybox, debug_texture;
    GlDungeon m_dungeon;
    std::list<std::shared_ptr<GlCharacter>>  dungeon_objects;
    std::list<std::shared_ptr<IMapEvent>> map_events;
    std::list<std::shared_ptr<IMapEvent>> hero_events;
    std::list<std::shared_ptr<IMapEvent>> mob_events;
    std::list<std::string> m_messages;

    std::map<std::string,std::vector<std::string>> m_scripts;
    
    std::shared_ptr<GlCharacter> hero;
    std::shared_ptr<GlCharacter> mob;
    std::string m_level_file;
    std::shared_ptr<Gl2D::GlImage> m_intro;
    
    bool m_antialiase_enabled;
    std::string m_start_place;
    glLight Light,Light2;
    float light_angle;
    float light_radius;
    float camera_distance;
    GlScene::glCamera Camera;
    int now_frame;
    double time;
    float key_angle;
    float camera_rotation_angle;
    float camera_height;
    float old_joy_x;
    float old_joy_y;

    std::pair<AnimationCommand, glm::mat4> unit_control_action;

    float m_daytime_in_hours;
    
    glm::vec3 hero_position;
    glm::vec3 light_position;
    glm::vec3 light_dir_vector;
    glm::vec3 light_color_vector;
    std::string m_info_message;

    LoaderUtility::LinesProcessor m_message_processor;


    
    void SaveObjects(const std::string &filename);

    void LoadMap(const std::string &filename,const std::string &start_place);
    void SelectStart(std::vector<std::string> &lines);
    void LoadTiles(std::vector<std::string> &lines);
    void LoadDungeonObjects(std::vector<std::string> &lines);
    void LoadScript(std::vector<std::string> &lines);
    void SetHeightmap(std::vector<std::string> &lines);
    
    bool AddObjectsFromFile(const std::string & object);
    bool AddObjectFromFile(const std::string & object,const std::string & name,glm::vec3 position);
    
    std::shared_ptr<GlCharacter> LoadObject(std::vector<std::string> &lines);
    void SetDungeonSize(std::vector<std::string> &lines);
    void LoadMapEvent(std::vector<std::string> &lines);

    void DrawGlobalCascade(const glRenderTargetDeffered &render_target);
    void PrerenderLight(glLight &Light,std::shared_ptr<GlCharacter>hero);
    void DrawGlobalLight(const GLuint light_loc, const glLight &Light);

    void DrawDungeon(GLuint &current_shader,  std::shared_ptr<GlCharacter>hero, const GlScene::glCamera &camera);
    void DrawHeightMap(GLuint current_shader, std::shared_ptr<GlCharacter>hero,const GlScene::glCamera &camera);
    
    void DrawLight(const glm::vec4 &light_pos_vector,const glm::vec3 &light_color_vector,glRenderTargetDeffered &render_target );
    void Draw2D(GLuint depth_map);

    void FitObjects(int steps, float accuracy);
    std::pair<float,const glm::vec3>  FitObjectToMap(GlCharacter& object);
    float FitObjectToObject(IGlModel& object1,IGlModel& object2);
    InteractionResult ReactObjectToEvent(std::weak_ptr<GlCharacter> object,IMapEvent& event,std::string &return_value);

    void DrawFxSprite(GLuint &current_shader, GLuint texture);
    void SetMapLight(std::vector<std::string> &lines);
    void MapObjectsEventsInteract();
    bool HeroEventsInteract(std::shared_ptr<GlCharacter> hero_ptr);
    bool MobKilled(std::shared_ptr<GlCharacter> obj);
    void PostMessage(const std::string & event_string);
    void ProcessMessages();
    std::pair<AnimationCommand,const glm::mat4> ProcessInputs(std::map <int, bool> &inputs);
    void ProcessInputsCamera(std::map <int, bool> &inputs,float joy_x, float joy_y);
    void ControlUnit(GlCharacter & character);
    std::shared_ptr<GlCharacter> MobPointer(const std::string & name);
    
    
};

#endif
