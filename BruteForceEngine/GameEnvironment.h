#ifndef GAME_ENVIRONMENT_H
#define GAME_ENVIRONMENT_H

#include "PlatformDefine.h"

namespace BruteForce
{
    namespace GlobalLevelInfo
    {
        struct AtmosphereInfo
        {
            //Math::Vec4Float m_PlanesPositions[1024];
            Math::Vec4Float m_SunInfo; // x,y,z - position, w - intensivity 
            Math::Vec4Float m_SunShadow; // x,y - 1,2 shadow decrease, z, w - lateral sun light direction 
            Math::Vec4Float m_SunColor; // x,y - 1,2 shadow decrease, z, w - lateral sun light direction 
            float m_SunShadowScaler;
            //uint32_t m_PlanesCount;
        };
        struct TerrainInfo
        {
            Math::Vec4Float m_TerrainScaler;
        };

        TerrainInfo& GetGlobalTerrainInfo();
        const TerrainInfo& ReadGlobalTerrainInfo();

        AtmosphereInfo& GetGlobalAtmosphereInfo();
        const AtmosphereInfo& ReadGlobalAtmosphereInfo();

        // ConstantBuffer<AtmosphereCB>* GetAtmosphereCB(uint8_t frame_index);
    }
}
#endif // !GAME_ENVIRONMENT_H

