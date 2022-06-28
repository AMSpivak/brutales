#ifndef INDEXED_GEOMETRY_GENERATOR_H
#define INDEXED_GEOMETRY_GENERATOR_H
#include "IndexedGeometry.h"
#include "Resources.h"

namespace BruteForce
{
    namespace Geometry
    {
        void CreateCube(Device& device, IndexedGeometry& geometry);


        template <class vertex_type>
        void CreatePlane(Device& device, IndexedGeometry& geometry, size_t cells_x, size_t cells_z, float scale_x, float scale_z)
        {
            //using vertex_type = VertexPosColor;

            size_t num_vertex = (cells_x + 1) * (cells_z + 1) + cells_x * 2 + cells_z * 2;
            size_t num_indexes = 3 * 2 * (cells_x) * (cells_z)+3 * 2 * 3 * cells_x + 3 * 2 * 2 * (cells_z);

            vertex_type* plane_vertices = new vertex_type[num_vertex];
            WORD* plane_indexes = new WORD[num_indexes];

            float step_z = scale_z * 2.0f / cells_z;
            float step_x = scale_x * 2.0f / cells_x;

            size_t low_point_index = 0;
            {
                size_t offset = 0;
                for (size_t i_z = 0; i_z <= cells_z; i_z++)
                {
                    //size_t offset = i_z * (cells_x + 1);
                    for (size_t i_x = 0; i_x <= cells_x; i_x++)
                    {
                        plane_vertices[offset].Position = { -scale_x + i_x * step_x, 0.0f, -scale_z + i_z * step_z };
                        //plane_vertices[offset].Color = { 0.0f, 0.0f, 0.0f };
                        ++offset;
                    }
                }

                low_point_index = offset;

                {
                    size_t i_z = 0;
                    for (size_t i_x = 0; i_x <= cells_x; i_x++)
                    {
                        plane_vertices[offset].Position = { -scale_x + i_x * step_x, -1.0f, -scale_z + i_z * step_z };
                        //plane_vertices[offset].Color = { 0.0f, 0.0f, 0.0f };
                        ++offset;
                    }

                    i_z = cells_z;
                    for (size_t i_x = 0; i_x <= cells_x; i_x++)
                    {
                        plane_vertices[offset].Position = { -scale_x + i_x * step_x, -1.0f, -scale_z + i_z * step_z };
                        //plane_vertices[offset].Color = { 0.0f, 0.0f, 0.0f };
                        ++offset;
                    }
                }
                {

                    for (size_t i_z = 1; i_z < cells_z; i_z++)
                    {
                        size_t i_x = 0;

                        plane_vertices[offset].Position = { -scale_x + i_x * step_x, -1.0f, -scale_z + i_z * step_z };
                        //plane_vertices[offset].Color = { 0.0f, 0.0f, 0.0f };
                        ++offset;

                        i_x = cells_x;
                        plane_vertices[offset].Position = { -scale_x + i_x * step_x, -1.0f, -scale_z + i_z * step_z };
                        //plane_vertices[offset].Color = { 0.0f, 0.0f, 0.0f };
                        ++offset;
                    }


                }
            }

            /*{
                size_t i_z = 0;
                size_t offset = i_z * (cells_x + 1);
                for (size_t i_x = 0; i_x <= cells_x; i_x++)
                {
                    plane_vertices[i_x + offset].Position = { -scale_x + i_x * step_x, -1.0f, -scale_z + i_z * step_z };
                    plane_vertices[i_x + offset].Color = { -scale_x + i_x * step_x, -scale_z + i_z * step_z, 0.0f };
                }
            }*/

            {
                size_t offset = 0;
                for (size_t i_z = 0; i_z < cells_z; i_z++)
                {
                    size_t offset_z = i_z * (cells_x + 1);
                    for (size_t i_x = 0; i_x < cells_x; i_x++)
                    {
                        size_t offset_xz = offset_z + i_x;
                        plane_indexes[offset++] = static_cast<WORD>(offset_xz);
                        plane_indexes[offset++] = static_cast<WORD>(offset_xz + cells_x + 1);
                        plane_indexes[offset++] = static_cast<WORD>(offset_xz + cells_x + 2);
                        plane_indexes[offset++] = static_cast<WORD>(offset_xz + cells_x + 2);
                        plane_indexes[offset++] = static_cast<WORD>(offset_xz + 1);
                        plane_indexes[offset++] = static_cast<WORD>(offset_xz);
                    }
                }
                {
                    size_t low_offset = low_point_index;
                    for (size_t i_x = 0; i_x < cells_x; i_x++)
                    {
                        plane_indexes[offset++] = static_cast<WORD>(i_x);
                        plane_indexes[offset++] = static_cast<WORD>(i_x + 1);
                        plane_indexes[offset++] = static_cast<WORD>(i_x + low_offset);
                        plane_indexes[offset++] = static_cast<WORD>(i_x + low_offset + 1);
                        plane_indexes[offset++] = static_cast<WORD>(i_x + low_offset);
                        plane_indexes[offset++] = static_cast<WORD>(i_x + 1);
                    }
                    low_offset = low_point_index + (cells_x + 1);

                    size_t offs = low_point_index - cells_x - 1;
                    for (size_t i_x = 0; i_x < cells_x; i_x++)
                    {

                        plane_indexes[offset++] = static_cast<WORD>(i_x + offs);
                        plane_indexes[offset++] = static_cast<WORD>(i_x + low_offset);
                        plane_indexes[offset++] = static_cast<WORD>(i_x + 1 + offs);
                        plane_indexes[offset++] = static_cast<WORD>(i_x + low_offset + 1);
                        plane_indexes[offset++] = static_cast<WORD>(i_x + 1 + offs);
                        plane_indexes[offset++] = static_cast<WORD>(i_x + low_offset);
                    }


                    size_t zd = cells_x + 1;
                    offs = zd * 2 + low_point_index;
                    size_t i_z = 0;

                    plane_indexes[offset++] = static_cast<WORD>(0 + cells_x);
                    plane_indexes[offset++] = static_cast<WORD>(offs + 1);
                    plane_indexes[offset++] = static_cast<WORD>(zd + cells_x);



                    for (i_z = 1; i_z < cells_z - 1; i_z++)
                    {
                        plane_indexes[offset++] = static_cast<WORD>(i_z * zd + cells_x);
                        plane_indexes[offset++] = static_cast<WORD>((i_z + 1) * zd + cells_x);
                        plane_indexes[offset++] = static_cast<WORD>((i_z - 1) * 2 + offs + 1);


                        plane_indexes[offset++] = static_cast<WORD>((i_z - 1) * 2 + offs + 1);
                        plane_indexes[offset++] = static_cast<WORD>((i_z + 1) * zd + cells_x);
                        plane_indexes[offset++] = static_cast<WORD>(i_z * 2 + offs + 1);
                    }

                    i_z = cells_z - 1;
                    plane_indexes[offset++] = static_cast<WORD>(i_z * zd + cells_x);
                    plane_indexes[offset++] = static_cast<WORD>((i_z + 1) * zd + cells_x);
                    plane_indexes[offset++] = static_cast<WORD>((i_z - 1) * 2 + offs + 1);




                    plane_indexes[offset++] = 0;
                    plane_indexes[offset++] = static_cast<WORD>(offs);
                    plane_indexes[offset++] = static_cast<WORD>(zd);

                    for (i_z = 1; i_z < cells_z - 1; i_z++)
                    {
                        plane_indexes[offset++] = static_cast<WORD>(i_z * zd);
                        plane_indexes[offset++] = static_cast<WORD>((i_z - 1) * 2 + offs);
                        plane_indexes[offset++] = static_cast<WORD>((i_z + 1) * zd);

                        plane_indexes[offset++] = static_cast<WORD>((i_z - 1) * 2 + offs);
                        plane_indexes[offset++] = static_cast<WORD>(i_z * 2 + offs);
                        plane_indexes[offset++] = static_cast<WORD>((i_z + 1) * zd);
                    }
                    i_z = cells_z - 1;
                    plane_indexes[offset++] = static_cast<WORD>(i_z * zd);
                    plane_indexes[offset++] = static_cast<WORD>((i_z - 1) * 2 + offs);
                    plane_indexes[offset++] = static_cast<WORD>((i_z + 1) * zd);

                }
            }



            SmartCommandQueue smart_queue(device, BruteForce::CommandListTypeDirect);
            auto commandList = smart_queue.GetCommandList();

            BruteForce::pResource intermediateVertexBuffer;
            BruteForce::UpdateBufferResource(device, commandList,
                &geometry.m_VertexBuffer, &intermediateVertexBuffer,
                num_vertex, sizeof(vertex_type), plane_vertices);

            geometry.m_VertexBufferView.BufferLocation = geometry.m_VertexBuffer->GetGPUVirtualAddress();
            geometry.m_VertexBufferView.SizeInBytes = static_cast<UINT>(sizeof(vertex_type) * num_vertex);
            geometry.m_VertexBufferView.StrideInBytes = static_cast<UINT>(sizeof(vertex_type));


            BruteForce::pResource intermediateIndexBuffer;
            BruteForce::UpdateBufferResource(device, commandList,
                &geometry.m_IndexBuffer, &intermediateIndexBuffer,
                num_indexes, sizeof(WORD), plane_indexes);

            geometry.m_IndexBufferView.BufferLocation = geometry.m_IndexBuffer->GetGPUVirtualAddress();
            geometry.m_IndexBufferView.Format = TargetFormat_R16_UInt;
            geometry.m_IndexBufferView.SizeInBytes = static_cast<UINT>(sizeof(WORD) * num_indexes);


            auto fenceValue = smart_queue.ExecuteCommandList(commandList);
            smart_queue.WaitForFenceValue(fenceValue);
            geometry.m_IndexesCount = num_indexes;
            intermediateIndexBuffer->Release();
            intermediateVertexBuffer->Release();
            delete[] plane_vertices;
            delete[] plane_indexes;
        }
        //void CreatePlane(Device& device, IndexedGeometry& geometry, size_t cells_x, size_t cells_y, float scale_x, float scale_y);
    }
}
#endif
