#ifndef SCENE_H
#define SCENE_H
#include "RenderObject.h"
#include <memory>
#include <vector>

namespace BruteForce
{
    using ScreneObject = Render::RenderObject;
    class Scene
    {
        std::vector<ScreneObject> m_StaticObjects;
    public:
        std::vector<ScreneObject>& GetStaticObjectsUnsafe();
        ScreneObject & AddStaticObjectUnsafe();
    };
}

#endif
