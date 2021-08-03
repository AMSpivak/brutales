#ifndef GL_ENGINE_PHYSICS
#define GL_ENGINE_PHYSICS

#include "collision.h"
namespace Physics
{
    template <typename   T1, typename T2>
    std::pair<float,glm::vec3> Intersection(T1 &object1,T2 &object2)
    {
        std::vector < glm::vec3 > axes;
        
        glm::vec3 o1 = object2.GetPosition() - object1.GetPosition();

        axes.push_back(glm::normalize(o1));

        object1.AddAxes(axes);
        object2.AddAxes(axes);

        glm::vec3 compensate_axe(0.0f,0.0f,0.0f);
        float intersection = std::numeric_limits<float>::max();

        for(const auto& axe : axes){
            //axe = glm::normalize(axe);
            std::pair<float,float> projection1 = object1.ProjectOnAxe(axe);
            std::pair<float,float> projection2 = object2.ProjectOnAxe(axe);
            float axe_intersection = Collision::CollisionOnAxe(projection1,projection2);

            if(axe_intersection < std::numeric_limits<float>::min())
                return std::make_pair((float)0.0f,compensate_axe);

            if(axe_intersection < intersection){
                compensate_axe = axe;
                intersection = axe_intersection;
            }
        }
        return std::make_pair(intersection,compensate_axe);
    }

    template <typename   T1, typename T2>
    float Collide(T1& object1, T2& object2)
    {
        float mass_summ = object1.mass_inv + object2.mass_inv;

        if (mass_summ < std::numeric_limits<float>::min())
            return 0.0f;

        auto intersection = Intersection(object1, object2);

        if (intersection.first < std::numeric_limits<float>::min())
            return 0.0f;

        float pos2_axe = glm::dot(object2.GetPosition() - object1.GetPosition(), intersection.second);
        //intersection.second[1] = 0.0f;
        if (pos2_axe < 0.0f)
        {
            intersection.first = -intersection.first;
        }

        float obj1_part = object1.mass_inv / mass_summ;
        float obj2_part = 1.0f - obj1_part;

        object1.SetPosition(object1.GetPosition() - obj1_part * intersection.first);
        object2.SetPosition(object2.GetPosition() + obj1_part * intersection.first);

        return intersection.first;
    }

}

#endif