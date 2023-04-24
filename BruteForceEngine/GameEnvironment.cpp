#include "GameEnvironment.h"

namespace BruteForce
{
    namespace GlobalLevelInfo
    {

        static TerrainInfo gTerrainInfo = {
            {0.0002f,200.0f, 0.0002f,  0}
        };

        TerrainInfo& GetGlobalTerrainInfo()
        {
            return gTerrainInfo;
            // TODO: �������� ����� �������� return
        }

        const TerrainInfo& ReadGlobalTerrainInfo()
        {
            return gTerrainInfo;
            // TODO: �������� ����� �������� return
        }

        static AtmosphereInfo gAtmosphereInfo = {
            {0.000f, 1.0f, 1.0f,  100.0f },
            {1.000f, 1.01f, 0.0f,  0.0f },
            {1.0f, 1.0f, 1.0f,  100.0f },
			{0.000f, 1.0f, 1.0f,  100.0f },
			{1.000f, 1.01f, 0.0f,  0.0f },
			{1.0f, 1.0f, 1.0f,  100.0f },
			1.0f,
			1.0f,
            false
        };

        AtmosphereInfo& GetGlobalAtmosphereInfo()
        {
            return gAtmosphereInfo;// TODO: �������� ����� �������� return
        }
        const AtmosphereInfo& ReadGlobalAtmosphereInfo()
        {
            return gAtmosphereInfo;// TODO: �������� ����� �������� return
        }

        static GameCameraInfo gGameCameraInfo;

        GameCameraInfo& GetGlobalGameCameraInfo()
        {
            return gGameCameraInfo;
        }

        const GameCameraInfo& ReadGlobalGameCameraInfo()
        {
            return gGameCameraInfo;
        }
    }
}