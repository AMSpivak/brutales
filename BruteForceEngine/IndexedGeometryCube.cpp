#include "IndexedGeometryGenerator.h"
#include "EngineGpuCommands.h"
#include "Resources.h"

using vec3f = BruteForce::Math::Vec3Float;
using vec2f = BruteForce::Math::Vec2Float;
using vec4f = BruteForce::Math::Vec4Float;
static BruteForce::VertexPosUvNormTangent cube_Vertices[8] = {
    { vec3f(-1.0f, -1.0f, -1.0f), vec2f(0.0f, 0.0f), vec3f(1.0f, 0.0f, 0.0f), vec4f(0.0f, 1.0f, 0.0f, 0.0f)  }, // 0
    { vec3f(-1.0f,  1.0f, -1.0f), vec2f(0.0f, 1.0f), vec3f(1.0f, 0.0f, 0.0f), vec4f(0.0f, 1.0f, 0.0f, 0.0f)  }, // 1
    { vec3f(1.0f,  1.0f, -1.0f),  vec2f(1.0f, 1.0f), vec3f(1.0f, 0.0f, 0.0f), vec4f(0.0f, 1.0f, 0.0f, 0.0f)  }, // 2
    { vec3f(1.0f, -1.0f, -1.0f),  vec2f(1.0f, 0.0f), vec3f(1.0f, 0.0f, 0.0f), vec4f(0.0f, 1.0f, 0.0f, 0.0f)   }, // 3
    { vec3f(-1.0f, -1.0f,  1.0f), vec2f(0.0f, 0.0f), vec3f(1.0f, 0.0f, 0.0f), vec4f(0.0f, 1.0f, 0.0f, 0.0f)   }, // 4
    { vec3f(-1.0f,  1.0f,  1.0f), vec2f(0.0f, 1.0f), vec3f(1.0f, 0.0f, 0.0f), vec4f(0.0f, 1.0f, 0.0f, 0.0f)   }, // 5
    { vec3f(1.0f,  1.0f,  1.0f),  vec2f(1.0f, 1.0f), vec3f(1.0f, 0.0f, 0.0f), vec4f(0.0f, 1.0f, 0.0f, 0.0f)   }, // 6
    { vec3f(1.0f, -1.0f,  1.0f),  vec2f(1.0f, 0.0f), vec3f(1.0f, 0.0f, 0.0f), vec4f(0.0f, 1.0f, 0.0f, 0.0f)   }  // 7
};

static WORD cube_Indicies[36] =
{
    0, 1, 2, 0, 2, 3,
    4, 6, 5, 4, 7, 6,
    4, 5, 1, 4, 1, 0,
    3, 2, 6, 3, 6, 7,
    1, 5, 6, 1, 6, 2,
    4, 0, 3, 4, 3, 7
};

namespace BruteForce
{
    namespace Geometry
    {
        void CreateCube(Device& device, IndexedGeometry& geometry)
        {
            CreateGeometry<BruteForce::VertexPosUvNormTangent>(device, geometry, reinterpret_cast<float*>(cube_Vertices), _countof(cube_Vertices), cube_Indicies, _countof(cube_Indicies));
            return;
            BruteForce::CreateBufferResource(device, &geometry.m_VertexBuffer, _countof(cube_Vertices), sizeof(BruteForce::VertexPosUvNormTangent));

            SmartCommandQueue smart_queue(device, BruteForce::CommandListTypeDirect);
            auto commandList = smart_queue.GetCommandList();
            BruteForce::pResource intermediateVertexBuffer;
            BruteForce::UpdateBufferResource(device, commandList,
                &geometry.m_VertexBuffer, &intermediateVertexBuffer,
                _countof(cube_Vertices), sizeof(BruteForce::VertexPosUvNormTangent), cube_Vertices);

            geometry.m_VertexBufferView.BufferLocation = geometry.m_VertexBuffer->GetGPUVirtualAddress();
            geometry.m_VertexBufferView.SizeInBytes = sizeof(cube_Vertices);
            geometry.m_VertexBufferView.StrideInBytes = sizeof(VertexPosUvNormTangent);

            BruteForce::CreateBufferResource(device, &geometry.m_IndexBuffer, _countof(cube_Indicies), sizeof(WORD));

            BruteForce::pResource intermediateIndexBuffer;
            BruteForce::UpdateBufferResource(device, commandList,
                &geometry.m_IndexBuffer, &intermediateIndexBuffer,
                _countof(cube_Indicies), sizeof(WORD), cube_Indicies);

            geometry.m_IndexBufferView.BufferLocation = geometry.m_IndexBuffer->GetGPUVirtualAddress();
            geometry.m_IndexBufferView.Format = TargetFormat_R16_UInt;
            geometry.m_IndexBufferView.SizeInBytes = sizeof(cube_Indicies);


            auto fenceValue = smart_queue.ExecuteCommandList(commandList);
            smart_queue.WaitForFenceValue(fenceValue);
            geometry.m_IndexesCount = _countof(cube_Indicies);
            intermediateIndexBuffer->Release();
            intermediateVertexBuffer->Release();
        }
    }


}