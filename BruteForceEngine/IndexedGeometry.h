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
        PosUvNormTangent,
        PosUvNormTangentBoneWeight
    };

    struct VertexPosColor
    {
        BruteForce::Math::Vec3Float Position;
        BruteForce::Math::Vec3Float Color;
        size_t VertexSize() { return sizeof(*this); }
    };

    struct VertexPosUvNormTangent
    {
        BruteForce::Math::Vec3Float Position;
        BruteForce::Math::Vec2Float Uv;
        BruteForce::Math::Vec3Float Normal;
        BruteForce::Math::Vec4Float Tangent;
        VertexPosUvNormTangent() {};
        VertexPosUvNormTangent(
            BruteForce::Math::Vec3Float position,
            BruteForce::Math::Vec2Float uv,
            BruteForce::Math::Vec3Float normal,
            BruteForce::Math::Vec4Float tangent
        ):Position(position)
            , Uv(uv)
            , Normal(normal)
            , Tangent(tangent)
        {};
        size_t VertexSize() { return sizeof(*this); }

    };

    struct VertexPosUvNormTangentBoneWeight
    {
        BruteForce::Math::Vec3Float Position;
        BruteForce::Math::Vec2Float Uv;
        BruteForce::Math::Vec3Float Normal;
        BruteForce::Math::Vec4Float Tangent;
        BruteForce::Math::Vec4UByte BoneIndex;
        BruteForce::Math::Vec4Float BoneWeight;
        VertexPosUvNormTangentBoneWeight() {};
        VertexPosUvNormTangentBoneWeight(
            BruteForce::Math::Vec3Float position,
            BruteForce::Math::Vec2Float uv,
            BruteForce::Math::Vec3Float normal,
            BruteForce::Math::Vec4Float tangent,
            BruteForce::Math::Vec4UByte bone_index,
            BruteForce::Math::Vec4Float bone_weight

        ) :Position(position)
            , Uv(uv)
            , Normal(normal)
            , Tangent(tangent)
            , BoneIndex(bone_index)
            , BoneWeight(bone_weight)
        {};
        size_t VertexSize() { return sizeof(*this); }
    };


    struct VertexPosNormTex
    {
        BruteForce::Math::Vec3Float Position;
        BruteForce::Math::Vec3Float Normal;
        //BruteForce::Math::Vec2Float Texture;
        size_t VertexSize() { return sizeof(*this); }

    };

    struct VertexPos
    {
        BruteForce::Math::Vec3Float Position;
        size_t VertexSize() { return sizeof(*this); }
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
