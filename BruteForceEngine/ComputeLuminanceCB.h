#ifndef COMPUTE_LUMINANCE_CB_H
#define COMPUTE_LUMINANCE_CB_H
//#include "ShaderCommon.h"
#include "ShaderCppTypesRedefine.h"
struct ComputeLuminanceCB
{
//    float4 LightSpace1; // x,y - direction x; z,w - offset
//   float4 LightSpace2; // x -reserved,y - height scale; z -first shadow height decrease, w - second shadow height decrease
    int srcTextureSize;

};
#endif

