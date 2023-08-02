#ifndef MATERIAL_CB_H
#define MATERIAL_CB_H
//#include "ShaderCommon.h"
#include "ShaderCppTypesRedefine.h"
#define MaterialCB_SIZE 1024
struct MaterialCB
{
    int4 Materials[MaterialCB_SIZE]; // x,y,z - map indexes; w - flags
};
#endif
