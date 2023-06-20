#ifndef RAYMARCH_FOG_CB_H
#define RAYMARCH_FOG_CB_H
#include "ShaderCppTypesRedefine.h"

#define DEFFERED_DISPATCH_SIZE 8

struct RaymarchFogCB
{
	float4 m_ScatteringParams0; //rgb -wavelengh scatter, a - transmittance coef
	float4 m_ScatteringParams1; //0 type: classic  r - height exp g - height zero ;1 type (rgb -position, a - g) ; w - g
	float4 m_ScatteringParams2; //r -extinction g - w0 b - w1 w -g
	float4 m_ScatteringParams3; //rgb -wavelengh absorption, a - reserved
	int4 m_Types; //r - scatter type: 0-Rayleight, 1 -Mie; g - fog volume type 0 - classic
};
#endif

