#ifndef GL_I_MAP_EVENT
#define GL_I_MAP_EVENT
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "gl_character.h"
#include "glcamera.h"

#include <vector>

enum class InteractionResult { Nothing, PostMessage, Damage, Kill, Use };
enum class EventProcessResult { Nothing, Kill, Used };

class IMapEvent
{
private:
protected:
    std::weak_ptr<GlCharacter> m_owner;

public:
    glm::mat4 model_matrix;
    float radius;
    float hero_dependence;
    float mobs_dependence;
    glm::vec3 position;
    IMapEvent():model_matrix(),radius(1.0f), position(0.0f,0.0f,0.0f)
    {

    }
    void SetPosition(const glm::vec3 &value)
    {
        position = value;
    }

    const glm::vec3 &GetPosition() const
    {
        return position;
    }

    void SetOwner(std::weak_ptr<GlCharacter> owner)
    {
        m_owner = owner;
    }

    virtual ~IMapEvent(){}

    virtual InteractionResult Interact(std::weak_ptr<GlCharacter> model,std::string &return_value) = 0;
    virtual EventProcessResult Process() = 0;
    virtual bool IsLight(glm::vec4 &light_position,glm::vec3 &color) {return false;}
    virtual void Show(const glm::vec3 & offset, GlScene::glCamera & camera){};
    virtual int AddAxes(std::vector<glm::vec3> &axes) = 0;
    virtual std::pair<float, float> ProjectOnAxe(const glm::vec3 & axe) = 0;
    virtual bool IsInteractable(std::weak_ptr<GlCharacter> obj){return true;}

};

#endif