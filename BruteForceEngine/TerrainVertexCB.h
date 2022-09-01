#ifndef TERRAIN_VERTEX_CB_H
#define TERRAIN_VERTEX_CB_H
struct TerrainVertexCB
{
    float4 m_PlanesPositions[1024];
    float4 m_TerrainScaler;
    float4 m_SunInfo;
    float4 m_SunShadow; // x,y - direction x; z,w - offset
    float4 m_SunColor; // w - intensivity
};
#endif
