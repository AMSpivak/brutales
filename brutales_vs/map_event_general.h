#ifndef GL_ME_GENERAL
#define GL_ME_GENERAL
#include "i_map_event.h"


class MapEventGeneral: public IMapEvent
{
private:
    std::vector<std::pair<glm::vec3, glm::vec3>> m_edges; 
    GLuint m_current_shader;
    GLuint m_depthmap;
    const GLuint * m_texture;
    float m_width; 
    float m_height;   
    float m_alpha;   
    float m_alpha_dec;
    std::string m_message;  

public:
    int frames_lifetime;
    MapEventGeneral(GLuint current_shader, GLuint depthmap,const GLuint * texture,
						 float width, float height):IMapEvent()
                            ,m_current_shader(current_shader)
                            ,m_depthmap(depthmap)
                            ,m_texture(texture)
                            ,m_width(width) 
                            ,m_height(height) 
                            ,m_alpha(1.0f) 
                            ,m_alpha_dec(0.001f)
                            ,m_message("")
    {    
    }

    ~MapEventGeneral(){}
    void SetMessage(std::string message);
    InteractionResult Interact(std::weak_ptr<GlCharacter> model,std::string &return_value);
    EventProcessResult Process();
    void Show(const glm::vec3 & offset, GlScene::glCamera & camera);
    bool IsLight(glm::vec4 &light_position,glm::vec3 &color);
    int AddAxes(std::vector<glm::vec3> &axes);
    void AddEdge(const std::pair<glm::vec3, glm::vec3> edge);
    std::pair<float, float> ProjectOnAxe(const glm::vec3 & axe);
};

#endif