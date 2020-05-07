#include "collision.h"
#include <iostream>
#include "glresourses.h"

namespace Collision
{
    std::pair<float, float> ProjectEdgesOnAxe( const glm::mat4 &model_matrix,const std::vector<std::pair<glm::vec3, glm::vec3>> &edges,const glm::vec3 &position,const glm::vec3 & axe)
    {
        float min = std::numeric_limits<float>::max();
        float max = std::numeric_limits<float>::min();
        glm::vec3 norm;
        glm::vec4 norm4;
        float val;
        for(auto edge :edges)
        {
            norm4 = model_matrix * glm::vec4(edge.second,0.0f);
            norm = position + glm::vec3(norm4) ;

            val = glm::dot(norm,axe);

            if(val < min) min = val;
            if(val > max) max = val;
        }
        return std::pair<float, float>(min, max);
        
    }

    int AddAxes(std::vector<glm::vec3> &axes,
                const std::vector<std::pair<glm::vec3, glm::vec3>> &m_edges,
                const glm::mat4 &model_matrix)
    {
        glm::vec4 norm4;
        glm::vec3 norm;
        glm::vec3 norm_z(0.0f,0.0f,1.0f);

        for(auto edge :m_edges)
        {
            norm4 = model_matrix * glm::vec4(edge.second,0.0f);
            norm = glm::vec3(norm4);        
            axes.push_back(glm::normalize(norm));
            norm = edge.second - edge.first;
            norm = glm::cross(norm_z,norm);
            norm4 = model_matrix * glm::vec4(norm,0.0f);
            norm = glm::vec3(norm4);
            axes.push_back(glm::normalize(norm));
        }
        
        return 2 * m_edges.size();
    }

    namespace _2d
    {
        double cross(glm::vec2 const& a, glm::vec2 const& b)
        {
            return a.x * b.y - b.x * a.y;
        }

        bool IsLeft(glm::vec2 const& a, glm::vec2 const& b)
        {
            float res = _2d::cross(a,b);
            //std::cout<<"\nIsLeft: "<<a <<" "<< b <<" "<<res<<"\n";
            return res > 0;
        }

        bool Intersect(const std::vector<glm::vec2> &frustrum,const C_2d_BB & bbox)
        {
            for(int i = 0; i < frustrum.size();i++)
            {
                int prev_i = i - 1;
                if(prev_i < 0)
                {
                    prev_i = frustrum.size() - 1;
                }
                glm::vec2 side = frustrum[i] - frustrum[prev_i];
                glm::vec2 box = bbox.center - frustrum[prev_i];

                bool all_right = !IsLeft(side,box +  bbox.size * glm::vec2(0.5f,0.5f));
                all_right &= !IsLeft(side,box +  bbox.size * glm::vec2(-0.5f,-0.5f));
                all_right &= !IsLeft(side,box +  bbox.size * glm::vec2(0.5f,-0.5f));
                all_right &= !IsLeft(side,box +  bbox.size * glm::vec2(-0.5f,0.5f));
                
                if(all_right) 
                    return false;
            }
            return true;

        }

    }

    
}