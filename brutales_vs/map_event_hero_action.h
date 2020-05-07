#ifndef GL_ME_HERO_ACTION
#define GL_ME_HERO_ACTION
#include "i_map_event.h"
#include "animation_sequence.h"

class IMapEventHeroAction: public IMapEvent
{
private:
    std::vector<std::pair<glm::vec3, glm::vec3>> m_edges; 
    float m_width; 
    float m_height;
    AnimationCommand m_command;   
public:
    float damage;
    int frames_lifetime;

    IMapEventHeroAction(float width, float height,AnimationCommand command):IMapEvent()
                            ,m_width(width) 
                            ,m_height(height)
                            ,m_command(command) 
    {
    }

    ~IMapEventHeroAction(){}

    InteractionResult Interact(std::weak_ptr<GlCharacter> model,std::string &return_value);
    EventProcessResult Process();
    void Show(const glm::vec3 & offset, GlScene::glCamera & camera);
    int AddAxes(std::vector<glm::vec3> &axes);
    void AddEdge(const std::pair<glm::vec3, glm::vec3> edge);
    std::pair<float, float> ProjectOnAxe(const glm::vec3 & axe);
};

#endif