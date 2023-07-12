#ifndef INDEXED_GEOMETRY_H
#define INDEXED_GEOMETRY_H
//#include "VideoDriverInterface.h"
#include "PlatformDefine.h"
namespace BruteForce
{
    enum class VertexFormat
    {
        Undefined,
        PosColor,
        PosNormTex,
        PosUvNormTangent
    };

    struct VertexPosColor
    {
        BruteForce::Math::Vec3Float Position;
        BruteForce::Math::Vec3Float Color;
    };

    struct VertexPosUvNormTangent
    {
        BruteForce::Math::Vec3Float Position;
        BruteForce::Math::Vec2Float Uv;
        BruteForce::Math::Vec3Float Normal;
        BruteForce::Math::Vec3Float Tangent;
        VertexPosUvNormTangent() {};
        VertexPosUvNormTangent(
            BruteForce::Math::Vec3Float position,
            BruteForce::Math::Vec2Float uv,
            BruteForce::Math::Vec3Float normal,
            BruteForce::Math::Vec3Float tangent
        ):Position(position)
            , Uv(uv)
            , Normal(normal)
            , Tangent(tangent)
        {};
    };

    struct VertexPosNormTex
    {
        BruteForce::Math::Vec3Float Position;
        BruteForce::Math::Vec3Float Normal;
        //BruteForce::Math::Vec2Float Texture;
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
        VertexFormat m_VertexFormat;
        IndexedGeometry() :m_VertexBuffer(nullptr), m_IndexBuffer(nullptr), m_IndexesCount(0), m_VertexFormat(VertexFormat::Undefined) {}
        ~IndexedGeometry()
        {
            if(m_VertexBuffer) m_VertexBuffer->Release();
            if(m_IndexBuffer) m_IndexBuffer->Release();
        }
    };
}

#endif
