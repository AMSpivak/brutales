#include "IndexedGeometryGenerator.h"
#include "EngineGpuCommands.h"
#include "Resources.h"

using vec3f = BruteForce::Math::Vec3Float;

namespace BruteForce
{
    namespace Geometry
    {
        void CreatePlane(Device& device, IndexedGeometry& geometry, size_t cells_x, size_t cells_z, float scale_x, float scale_z)
        {
            using vertex_type = VertexPosColor;

            size_t num_vertex = (cells_x + 1) * (cells_z + 1);
            size_t num_indexes = 3 * 2 * (cells_x) * (cells_z);

            vertex_type* plane_vertices = new vertex_type[num_vertex];
            WORD* plane_indexes = new WORD[num_indexes];

            float step_z = scale_z * 2.0f / cells_z;
            float step_x = scale_x * 2.0f / cells_x;

            for (size_t i_z = 0; i_z <= cells_z; i_z++)
            {
                size_t offset = i_z * (cells_x + 1);
                for (size_t i_x = 0; i_x <= cells_x; i_x++)
                {
                    plane_vertices[i_x + offset].Position = { -scale_x + i_x * step_x, 0.0f, -scale_z + i_z * step_z };
                    plane_vertices[i_x + offset].Color = { -scale_x + i_x * step_x, -scale_z + i_z * step_z, 0.0f};
                }
            }

            {
                using int_type = WORD;
                int_type offset = 0;
                for (int_type i_z = 0; i_z < cells_z; i_z++)
                {
                    int_type offset_z = i_z * (static_cast<int_type>(cells_x) + 1);
                    for (int_type i_x = 0; i_x < cells_x; i_x++)
                    {
                        int_type offset_xz = offset_z + i_x;
                        plane_indexes[offset++] = offset_xz;
                        plane_indexes[offset++] = offset_xz + 1;
                        plane_indexes[offset++] = offset_xz;
                    }
                }
            }

            SmartCommandQueue smart_queue(device, BruteForce::CommandListTypeDirect);
            auto commandList = smart_queue.GetCommandList();

            BruteForce::pResource intermediateVertexBuffer;
            BruteForce::UpdateBufferResource(device, commandList,
                &geometry.m_VertexBuffer, &intermediateVertexBuffer,
                num_vertex, sizeof(vertex_type), plane_vertices);

            geometry.m_VertexBufferView.BufferLocation = geometry.m_VertexBuffer->GetGPUVirtualAddress();
            geometry.m_VertexBufferView.SizeInBytes = sizeof(vertex_type) * num_vertex;
            geometry.m_VertexBufferView.StrideInBytes = sizeof(vertex_type);
            

            BruteForce::pResource intermediateIndexBuffer;
            BruteForce::UpdateBufferResource(device, commandList,
                &geometry.m_IndexBuffer, &intermediateIndexBuffer,
                num_indexes, sizeof(WORD), plane_indexes);

            geometry.m_IndexBufferView.BufferLocation = geometry.m_IndexBuffer->GetGPUVirtualAddress();
            geometry.m_IndexBufferView.Format = DXGI_FORMAT_R16_UINT;
            geometry.m_IndexBufferView.SizeInBytes = sizeof(WORD) * num_indexes;


            auto fenceValue = smart_queue.ExecuteCommandList(commandList);
            smart_queue.WaitForFenceValue(fenceValue);
            geometry.m_IndexesCount = num_indexes;
            intermediateIndexBuffer->Release();
            intermediateVertexBuffer->Release();
            delete[] plane_vertices;
            delete[] plane_indexes;
        }
    }


}