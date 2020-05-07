#ifndef COLLISION
#define COLLISION
#include <utility>
#include <vector>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
namespace Collision
{
    struct C_2d_BB
    {   
        glm::vec2 center;
        glm::vec2 size;  
    };

    struct C_2d_Line
    {   
        glm::vec2 point1;
        glm::vec2 point2;  
    };



    struct C_2d_Plane
    {   
        glm::vec2 point;
        glm::vec2 normal;  
    };

    


    inline float CollisionOnAxe(float start1,float end1,float start2,float end2)
    {
        if(start1 >end1)
            std::swap(start1, end1);
        if(start2 >end2)
            std::swap(start2, end2);

        float start = start1 > start2 ? start1 : start2;
        float end = end1 < end2 ? end1 : end2;

        return start < end ? end - start : 0;   
    }

    inline float CollisionOnAxe(std::pair<float,float> projection1,std::pair<float,float> projection2)
    {
        if(projection1.first >projection1.second)
            std::swap(projection1.first,projection1.second);
        if(projection2.first > projection2.second)
            std::swap(projection2.first,projection2.second);

        float start = projection1.first > projection2.first ? projection1.first : projection2.first;
        float end = projection1.second < projection2.second ? projection1.second : projection2.second;

        return start < end ? end - start : 0;   
    }

    std::pair<float, float> ProjectEdgesOnAxe( const glm::mat4 &model_matrix,const std::vector<std::pair<glm::vec3, glm::vec3>> &edges,const glm::vec3 &position,const glm::vec3 & axe);

    int AddAxes(std::vector<glm::vec3> &axes,
                const std::vector<std::pair<glm::vec3, glm::vec3>> &m_edges,
                const glm::mat4 &model_matrix);

    namespace _2d
    {

        bool IsLeft(glm::vec2 const& a, glm::vec2 const& b);
        bool Intersect(const std::vector<glm::vec2> &frustrum,const C_2d_BB & bbox);
    }
}

#endif