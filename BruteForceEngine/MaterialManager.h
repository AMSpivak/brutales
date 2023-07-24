#ifndef MATERIAL_MANAGER
#define MATERIAL_MANAGER
#include "PlatformDefine.h"
#include "Helpers.h"
#include "Atlas.h"
#include "Texture.h"
#include <string>
#include <vector>

namespace BruteForce
{
	struct RenderMaterial
	{
		//size_t m_Start;
		std::shared_ptr<Textures::Texture> m_Albedo;
		std::shared_ptr<Textures::Texture> m_Normals;
		std::shared_ptr<Textures::Texture> m_RoughnessMetalness;
		uint16_t m_flags;
		//DescriptorHandle m_CpuHandle;
		//DescriptorRangeType m_Type;
		std::string m_Name;

		RenderMaterial() :m_flags(0), m_Name("Unnamed") {}
		//DescriptorHeapRange(const std::string& name, DescriptorRangeType type) :m_Start(0), m_Size(0), m_CpuHandle(), m_Type(type), m_Name(name) {}

		//DescriptorHeapRange(size_t start, size_t size, DescriptorRangeType const std::string &name):m_Start(start), m_Size(size), m_Name(name) {}
		//void Fill(DescriptorRange& range, size_t reg);
		//void Fill(DescriptorRange& range, size_t internal_offset, size_t internal_size, size_t reg);
	};



	class MaterialManager
	{
	private:
		Atlas<Textures::Texture> m_TextureAtlas;
		std::shared_ptr<DescriptorHeapRange> m_TexturesSrvs;
		std::vector<std::shared_ptr<RenderMaterial>> m_Materials;
		size_t m_size;
		size_t m_index;
	public:
		MaterialManager() {};
		~MaterialManager();
		//DescriptorHeapManager() :m_HeapType(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV), m_size(0), m_index(0), m_TextureAtlas("") {}
		//void Create(Device& device, size_t size, DescriptorHeapType HeapType);
		//DescriptorHandle AllocateRange(Device& device, size_t size, DescriptorHeapRange& range);
		//std::shared_ptr<DescriptorHeapRange> AllocateManagedRange(Device& device, size_t size, DescriptorRangeType type, const std::string& name);
		//std::shared_ptr<DescriptorHeapRange> GetManagedRange(const std::string& name);
		//DescriptorHandleGpu GetGpuDescriptorHandle();
		//pDescriptorHeap GetDescriptorHeapPointer();
	};

}

#endif
