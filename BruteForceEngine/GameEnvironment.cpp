#include "GameEnvironment.h"

namespace BruteForce
{
    namespace GlobalLevelInfo
    {

        TerrainInfo gTerrainInfo = {
            {0.0002f,300.0f, 0.0002f,  0}
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
    }
}