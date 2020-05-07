#ifndef GL_I_GAME_EVENT_FABRIC
#define GL_I_GAME_EVENT_FABRIC
#include "i_map_event.h"
#include <memory>
namespace GameEvents
{
    enum class EventTypes {HeroStrike,HeroUse,BarrelValhalla};
    struct GeneralEventStruct
    {
        GlCharacter * object;
        GLuint current_shader;
        GLuint depthmap;
        const GLuint * texture;
    };

    struct GeneralEventStrike
    {
        std::shared_ptr<GlCharacter> source;
        float strike_force = 0.0f;
        const GLuint * texture = nullptr;
        GLuint current_shader = 0;
        GLuint depthmap = 0;
    };

    std::shared_ptr<IMapEvent> CreateGameEvent(EventTypes event_type, const void * parameters);
}
#endif