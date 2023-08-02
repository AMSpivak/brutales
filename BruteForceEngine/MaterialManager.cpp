#include "MaterialManager.h"
namespace BruteForce
{
    MaterialManager::MaterialManager(Device& device,
        SmartCommandQueue& copy_queue,
        GpuAllocator gpu_allocator,
        DescriptorHeapManager& descriptor_heap_manager,
        const std::wstring& ResourseFolder, size_t texture_capacity)
        :m_TextureAtlas(ResourseFolder)
        , m_TextureHelper(device, copy_queue, gpu_allocator)
        , m_MaterialBuffers(nullptr)
        , m_texture_capacity(texture_capacity)
        , m_size(0)
        , m_need_update_cbv(true)
    {
        m_TexturesSrvs = descriptor_heap_manager.AllocateManagedRange(device, m_texture_capacity, DescriptorRangeType::D3D12_DESCRIPTOR_RANGE_TYPE_SRV, "MaterialManagerTextures");
        if (m_MaterialBuffers)
        {
            delete[] m_MaterialBuffers;
        }
        int frames_count = 3;
        m_MaterialBuffers = new ConstantBuffer<MaterialCB>[frames_count];

        {
            m_MaterialsCbvRange = descriptor_heap_manager.AllocateManagedRange(device, static_cast<UINT>(frames_count), BruteForce::DescriptorRangeTypeCvb, "MaterialCBVs");
            auto& cvb_handle = m_MaterialsCbvRange->m_CpuHandle;//descriptor_heap_manager.AllocateRange(device, static_cast<UINT>(frames_count), CbvRange);

            for (int i = 0; i < frames_count; i++)
            {
                CreateUploadGPUBuffer(device, m_MaterialBuffers[i], cvb_handle);

                m_MaterialBuffers[i].Map();
                m_MaterialBuffers[i].Update();

                cvb_handle.ptr += device->GetDescriptorHandleIncrementSize(BruteForce::DescriptorHeapCvbSrvUav);
            }
        }
    }

    MaterialManager::~MaterialManager()
    {
        if (m_MaterialBuffers)
        {
            delete[] m_MaterialBuffers;
        }
    }

    //const std::shared_ptr<DescriptorHeapRange> MaterialManager::GetDescriptorRange()
    //{
    //    return m_TexturesSrvs;
    //}
    std::shared_ptr<RenderMaterial> MaterialManager::AddMaterial()
    {
        return m_Materials.emplace_back(std::make_shared<RenderMaterial>(m_Materials.size()));
    }
    std::shared_ptr<RenderMaterial> MaterialManager::AddMaterial(const std::wstring& albedo, const std::wstring& normal_height, const std::wstring& roughness_metalness)
    {
        auto& material = m_Materials.emplace_back(std::make_shared<RenderMaterial>(m_Materials.size()));
        
        material->m_Albedo = m_TextureAtlas.Assign(albedo, m_TextureHelper, *m_TexturesSrvs, m_size);
        if (material->m_Albedo->GetSrvRangeIndex() == m_size)
        {
            ++m_size;
        }

        material->m_Normals = m_TextureAtlas.Assign(normal_height, m_TextureHelper, *m_TexturesSrvs, m_size);
        if (material->m_Normals->GetSrvRangeIndex() == m_size)
        {
            ++m_size;
        }
        
        return material;
    }
    void MaterialManager::UpdateBuffer(int index)
    {
        for (int i = 0; i < MaterialCB_SIZE; i++)
        {
            if (i < m_Materials.size())
            {
                auto& mat = m_Materials[i];
                m_MaterialBuffers[index].m_CpuBuffer->Materials[i] = int4{ mat->m_Albedo->GetSrvRangeIndex(), mat->m_Normals->GetSrvRangeIndex(),0,0};
            }
            else
            {
                m_MaterialBuffers[index].m_CpuBuffer->Materials[i] = int4{0,0,0,0};
            }
            
        }

        m_MaterialBuffers[index].Update();
    }
}
