#include "GameEnvironment.h"

namespace BruteForce
{
    namespace GlobalLevelInfo
    {

        TerrainInfo gTerrainInfo = {
            {0.0002f,200.0f, 0.0002f,  0}
        };

        TerrainInfo& GetGlobalTerrainInfo()
        {
            return gTerrainInfo;
            // TODO: вставьте здесь оператор return
        }

        const TerrainInfo& ReadGlobalTerrainInfo()
        {
            return gTerrainInfo;
            // TODO: вставьте здесь оператор return
        }

        AtmosphereInfo gAtmosphereInfo = {
            {0.000f, 1.0f, 1.0f,  100.0f },
            {1.000f, 1.01f, 0.0f,  0.0f }
        };

        AtmosphereInfo& GetGlobalAtmosphereInfo()
        {
            return gAtmosphereInfo;// TODO: вставьте здесь оператор return
        }
        const AtmosphereInfo& ReadGlobalAtmosphereInfo()
        {
            return gAtmosphereInfo;// TODO: вставьте здесь оператор return
        }
    }
}