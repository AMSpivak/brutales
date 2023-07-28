#include "MaterialManager.h"
namespace BruteForce
{
    MaterialManager::MaterialManager(Device& device, DescriptorHeapManager& descriptor_heap_manager, const std::wstring& ResourseFolder, size_t texture_capacity):m_TextureAtlas(ResourseFolder), m_texture_capacity(texture_capacity), m_size(0), m_need_update_cbv(true)
    {
        m_TexturesSrvs = descriptor_heap_manager.AllocateManagedRange(device, m_texture_capacity, DescriptorRangeType::D3D12_DESCRIPTOR_RANGE_TYPE_SRV, "MaterialManagerTextures");
    }

    MaterialManager::~MaterialManager()
    {

    }

    const std::shared_ptr<DescriptorHeapRange> MaterialManager::GetDescriptorRange()
    {
        return m_TexturesSrvs;
    }
    std::shared_ptr<RenderMaterial> MaterialManager::AddMaterial()
    {
        return m_Materials.emplace_back(m_Materials.size());
    }
    std::shared_ptr<RenderMaterial> MaterialManager::AddMaterial(std::wstring& albedo, std::wstring& normal_height, std::wstring& roughness_metalness)
    {
        auto& material = m_Materials.emplace_back(m_Materials.size());
        
        material->m_Albedo = m_TextureAtlas.Assign(albedo, m_TextureHelper, m_TexturesSrvs, m_size);
        if (material->m_Albedo->GetSrvRangeIndex() == m_size)
        {
            ++m_size;
        }

        material->m_Normals = m_TextureAtlas.Assign(normal_height, m_TextureHelper, m_TexturesSrvs, m_size);
        if (material->m_Normals->GetSrvRangeIndex() == m_size)
        {
            ++m_size;
        }
        return material;
    }
}
