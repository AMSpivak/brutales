#include "gl_physics.h"
#include < algorithm >
namespace Physics
{
    void Constrain(glm::vec3& p1, glm::vec3& p2, float length, float force_in, float force_out, float weight1, float inv_mass2)
    {
        auto p12 = p2 - p1;
        float p12_l = glm::length(p12);
        float delta_l = length - p12_l;
        if (p12_l == 0)
            return;

        auto force = delta_l < 0 ? force_in : force_out;

        p12 = force * p12 * (delta_l / p12_l);
        weight1 = std::clamp(weight1, 0.0f, 1.0f);
        p2 += (1.0f - weight1) * p12;
        p1 -= weight1 * p12;
    }
}