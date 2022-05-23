#ifndef SHADER_COMMON_H
#define SHADER_COMMON_H
#ifdef __cplusplus
// C++ code
#define CONST_BUFFER(Name, Register) struct Name
#else
// HLSL code
#define CONST_BUFFER(Name, Register) cbuffer Name : register (b##Register)
#endif 
#endif 
