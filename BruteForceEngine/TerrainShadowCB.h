#ifndef TERRAIN_SHADOW_CB_H
#define TERRAIN_SHADOW_CB_H
#include "ShaderCommon.h"

struct TerrainShadowCB
{
    float4 LightSpace1; // x,y - direction x; z,w - offset
    float4 LightSpace2; // x -reserved,y - height scale; z -first shadow height decrease, w - second shadow height decrease
    int2 srcTextureSize;

};
#endif
