#ifndef MATERIAL_CB_H
#define MATERIAL_CB_H
//#include "ShaderCommon.h"
#include "ShaderCppTypesRedefine.h"
struct MaterialCB
{
    int4 Materials[1024]; // x,y,z - map indexes; w - flags
};
#endif
