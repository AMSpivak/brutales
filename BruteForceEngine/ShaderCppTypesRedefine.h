#ifdef __cplusplus
#ifndef SHADER_CPP_TYPES_REDEFINE_H
#define SHADER_CPP_TYPES_REDEFINE_H
#include "PlatformDefine.h"

#ifdef PLATFORM_DX12_H
//using float4 = BruteForce::Math::Vec4Float;
#define float4 BruteForce::Math::Vec4Float
#define int2 BruteForce::Math::Vec2Int
#endif

#endif
#endif
