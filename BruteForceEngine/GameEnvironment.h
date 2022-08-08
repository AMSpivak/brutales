#ifndef GAME_ENVIRONMENT_H
#define GAME_ENVIRONMENT_H

#include "PlatformDefine.h"

namespace BruteForce
{
    namespace GlobalLevelInfo
    {
        struct AtmosphereCB
        {
            //Math::Vec4Float m_PlanesPositions[1024];
            Math::Vec4Float m_SunDirection;
            //uint32_t m_PlanesCount;
        };
        struct TerrainInfo
        {
            Math::Vec4Float m_TerrainScaler;
        };

        TerrainInfo& GetGlobalTerrainInfo();
        const TerrainInfo& ReadGlobalTerrainInfo();

        // ConstantBuffer<AtmosphereCB>* GetAtmosphereCB(uint8_t frame_index);
    }
}
#endif // !GAME_ENVIRONMENT_H

