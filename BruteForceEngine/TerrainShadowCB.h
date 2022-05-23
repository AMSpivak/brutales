#ifndef TERRAIN_SHADOW_CB_H
#define TERRAIN_SHADOW_CB_H
#include "ShaderCommon.h"

CONST_BUFFER(TerrainShadowCB, 0)
{
    int2 srcTextureSize;
};
#endif
