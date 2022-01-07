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

    struct VertexPosNormTex
    {
        BruteForce::Math::Vec3Float Position;
        BruteForce::Math::Vec3Float Normal;
        BruteForce::Math::Vec2Float Texture;
    };

    struct VertexPos
    {
        BruteForce::Math::Vec3Float Position;
    };

    class IndexedGeometry
    {
    public:
        pResource m_VertexBuffer;
        VertexBufferView m_VertexBufferView;
        pResource m_IndexBuffer;
        IndexBufferView m_IndexBufferView;
        size_t m_IndexesCount;
        ~IndexedGeometry()
        {
            m_VertexBuffer->Release();
            m_IndexBuffer->Release();
        }
    };
}

#endif
