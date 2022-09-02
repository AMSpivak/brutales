#ifndef SKY_PIXEL_CB_H
#define SKY_PIXEL_CB_H

#include "ShaderCppTypesRedefine.h"

struct SkyPixelCB
{
    float4 LightDir;
    float4 LightColor;
    float4 SkyColor;
    float4 CamPosition;
    float4 MoonDirection;
    float4 MoonColor;
};
#endif
