#ifndef GAME_ENVIRONMENT_H
#define GAME_ENVIRONMENT_H

#include "PlatformDefine.h"
#include "ConstantBuffer.h"

namespace BruteForce
{
    namespace Atmosphere
    {
        struct AtmosphereCB
        {
            //Math::Vec4Float m_PlanesPositions[1024];
            Math::Vec4Float m_SunDirection;
            //uint32_t m_PlanesCount;
        };

        // ConstantBuffer<AtmosphereCB>* GetAtmosphereCB(uint8_t frame_index);
    }
}
#endif // !GAME_ENVIRONMENT_H

