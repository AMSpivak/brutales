#ifndef TONEMAP_CB_H
#define TONEMAP_CB_H
#include "ShaderCommon.h"

//CONST_BUFFER(TonemapCB, 0)
//{
//    float CB_Exposure;
//};

#include "ShaderCppTypesRedefine.h"
struct TonemapCB
{
    float Exposure;
    float Nits;
    int CurveType;
    int SourceFrameIndex;

};
#endif