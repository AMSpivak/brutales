#ifndef INDEXED_GEOMETRY_H
#define INDEXED_GEOMETRY_H
//#include "VideoDriverInterface.h"
#include "PlatformDefine.h"
namespace BruteForce
{
    class IndexedGeometry
    {
    public:
        Resource m_VertexBuffer;
        VertexBufferView m_VertexBufferView;
        Resource m_IndexBuffer;
        IndexBufferView m_IndexBufferView;
    };
}

#endif
