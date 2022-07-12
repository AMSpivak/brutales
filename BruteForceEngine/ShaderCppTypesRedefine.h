#ifndef SHADER_CPP_TYPES_REDEFINE_H
#define SHADER_CPP_TYPES_REDEFINE_H
#include "PlatformDefine.h"

#ifdef PLATFORM_DX12_H
using float4 = BruteForce::Math::Vec4Float;
#endif

#endif
