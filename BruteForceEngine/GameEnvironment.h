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
            Math::Vec4Float m_SunColor; // x,y,z - color, w - intensivity 
            Math::Vec4Float m_MoonInfo; // x,y,z - position, w - intensivity 
			Math::Vec4Float m_MoonShadow; // x,y - 1,2 shadow decrease, z, w - lateral moon light direction 
            Math::Vec4Float m_MoonColor; // x,y,z - color, w - intensivity 


			float m_SunShadowScaler;
			float m_MoonShadowScaler;

            bool m_Moonlight;
            //uint32_t m_PlanesCount;
        };
        struct TerrainInfo
        {
            Math::Vec4Float m_TerrainScaler;
        };

        struct GameCameraInfo
        {
            bool m_EyeAdapt;
            GameCameraInfo():m_EyeAdapt(true)
            {}
        };


        TerrainInfo& GetGlobalTerrainInfo();
        const TerrainInfo& ReadGlobalTerrainInfo();

        AtmosphereInfo& GetGlobalAtmosphereInfo();
        const AtmosphereInfo& ReadGlobalAtmosphereInfo();

        GameCameraInfo& GetGlobalGameCameraInfo();
        const GameCameraInfo& ReadGlobalGameCameraInfo();

        // ConstantBuffer<AtmosphereCB>* GetAtmosphereCB(uint8_t frame_index);
    }
}
#endif // !GAME_ENVIRONMENT_H

