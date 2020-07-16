#ifndef MATH3D_H
#define MATH3D_H
#include <iostream>
#include "glm/glm.hpp"

namespace Math3D
{
    enum class SimpleDirections {None, Left, Right, Forward, Back};
    float Disorientation(   const glm::vec3 & face_direction, 
                            const glm::vec3 & object_direction,
                            const glm::vec3 & positive_disorientation);

    SimpleDirections SimplifyDirection(float);

    bool IsCounterClockwiseTriangle(    const glm::vec3 & a1, 
                                    const glm::vec3 & a2,
                                    const glm::vec3 & a3,
                                    const glm::vec3 & normal
                                    );
    std::ostream& operator << ( std::ostream& os, const SimpleDirections & value);

    float  	TripleProduct(const glm::vec3& a1,
        const glm::vec3& a2,
        const glm::vec3& a3);
}
#endif