#ifndef TERRAIN_VERTEX_CB_H
#define TERRAIN_VERTEX_CB_H
#include "ShaderCommon.h"
//#include "ShaderCppTypesRedefine.h"
#define SHADER_MANUAL_BIND
CONST_BUFFER(TerrainCB, 0)
{
    float4 m_PlanesPositions[1024];
    float4 m_TerrainScaler;
};
#undef SHADER_MANUAL_BIND

#endif
