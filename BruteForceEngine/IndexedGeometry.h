#ifndef INDEXED_GEOMETRY_H
#define INDEXED_GEOMETRY_H
//#include "VideoDriverInterface.h"
#include "PlatformDefine.h"
namespace BruteForce
{
    struct VertexPosColor
    {
        BruteForce::Math::Vec3Float Position;
        BruteForce::Math::Vec3Float Color;
    };

    class IndexedGeometry
    {
    public:
        pResource m_VertexBuffer;
        VertexBufferView m_VertexBufferView;
        pResource m_IndexBuffer;
        IndexBufferView m_IndexBufferView;
    };
}

#endif
