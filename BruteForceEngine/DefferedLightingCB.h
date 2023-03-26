#ifndef DEFFERED_LIGHTING_CB_H
#define DEFFERED_LIGHTING_CB_H
#include "ShaderCppTypesRedefine.h"

#define DEFFERED_DISPATCH_SIZE 8

struct DefferedLightingCB
{
    matrix m_CameraInverse;
    float4 m_CameraPosition;
    float4 m_SunInfo;
    float4 m_SunShadow; // x,y - direction x; z,w - offset
    float4 m_SunColor; // w - intensivity
    float4 m_MoonInfo;
    float4 m_TerrainScaler; 
};
#endif

