#ifndef _BRUTEFORCE_CAMERA_H
#define _BRUTEFORCE_CAMERA_H
#include "PlatformDefine.h"

namespace BruteForce
{
    class Camera
    {
        Math::Matrix m_Projection;
        Math::Vec3Float Position;
        float m_Fov;
    };
}


#endif
