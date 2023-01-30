#ifndef DEFFERED_LIGHTING_CB_H
#define DEFFERED_LIGHTING_CB_H
struct DefferedLightingCB
{
    float4 m_SunInfo;
    float4 m_SunShadow; // x,y - direction x; z,w - offset
    float4 m_SunColor; // w - intensivity
};
#endif

