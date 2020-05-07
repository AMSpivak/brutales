#ifndef GL_ME_HERO_STRIKES
#define GL_ME_HERO_STRIKES
#include "i_map_event.h"


class IMapEventHeroStrike: public IMapEvent
{
private:
    std::vector<std::pair<glm::vec3, glm::vec3>> m_edges; 
    GLuint m_current_shader;
    GLuint m_depthmap;
    const GLuint * m_texture;
    float m_width; 
    float m_height;
    float m_damage;
    glm::vec3 m_indicator;
    glm::vec3 m_points[3];
public:

    int frames_lifetime;
    IMapEventHeroStrike(GLuint current_shader, GLuint depthmap, const GLuint * texture,
						 float width, float height, float damage):IMapEvent()
                            ,m_current_shader(current_shader)
                            ,m_depthmap(depthmap)
                            ,m_texture(texture)
                            ,m_width(width) 
                            ,m_height(height)
                            ,m_damage(damage) 
    {
    }

    ~IMapEventHeroStrike(){}

    InteractionResult Interact(std::weak_ptr<GlCharacter> model,std::string &return_value);
    EventProcessResult Process();
    void Show(const glm::vec3 & offset, GlScene::glCamera & camera);
    int AddAxes(std::vector<glm::vec3> &axes);
    void AddEdge(const std::pair<glm::vec3, glm::vec3> edge);
    std::pair<float, float> ProjectOnAxe(const glm::vec3 & axe);
    bool IsInteractable(std::weak_ptr<GlCharacter> obj);
    void SetIndicator(glm::vec3 indicator);
    void AssignPoints(  const glm::vec3 & p1,
                        const glm::vec3 & p2,
                        const glm::vec3 & p3);
};

#endif