#ifndef SHADER_COMMON_H
#define SHADER_COMMON_H
#ifdef __cplusplus
// C++ code
#define CONST_BUFFER(Name, Register) struct Name
#define CONST_BUFFER_ARRAY(Name, Register, length) struct Name
#else 
// HLSL code
#ifdef SHADER_MANUAL_BIND
#define CONST_BUFFER(Name, Register) struct Name
#define CONST_BUFFER_ARRAY(Name, Register, length) struct Name
#else
#define CONST_BUFFER(Name, Register) cbuffer Name : register (b##Register)
#define CONST_BUFFER_ARRAY(Name, Register, length) cbuffer Name [length] : register (b##Register)
#endif
#endif 
#endif 
