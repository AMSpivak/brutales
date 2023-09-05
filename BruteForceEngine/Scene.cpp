#include "Scene.h"
namespace BruteForce
{
    std::vector<ScreneObject>& Scene::GetStaticObjectsUnsafe()
    {
        return m_StaticObjects;
    }
    ScreneObject& Scene::AddStaticObjectUnsafe()
    {
        return m_StaticObjects.emplace_back();
    }
}