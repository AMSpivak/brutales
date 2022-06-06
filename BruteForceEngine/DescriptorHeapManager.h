#ifndef DESCRIPTOR_HEAP_MANAGER
#define DESCRIPTOR_HEAP_MANAGER
#include "PlatformDefine.h"
#include "Helpers.h"
#include <string>
#include <vector>

namespace BruteForce
{
	struct DescriptorHeapRange
	{
		size_t m_Start;
		size_t m_Size;
		DescriptorRangeType m_Type;
		std::string m_Name;
		//DescriptorHandle m_CpuHandle;
		DescriptorHeapRange() :m_Start(0), m_Size(0), m_Type(DescriptorRangeTypeSrv), m_Name("Unnamed") {}
		DescriptorHeapRange(DescriptorRangeType type, const std::string &name):m_Start(0), m_Size(0), m_Type(type), m_Name(name) {}

		//DescriptorHeapRange(size_t start, size_t size, DescriptorRangeType const std::string &name):m_Start(start), m_Size(size), m_Name(name) {}
		void Fill(DescriptorRange* range, size_t reg);
	};



	class DescriptorHeapManager
	{
	private:
		DescriptorHeapType m_HeapType;
		size_t m_size;
		size_t m_index;
		DescriptorHeap m_Heap;
	public:

		DescriptorHeapManager() :m_HeapType(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV), m_size(0) , m_index(0){}
		void Create(Device& device, size_t size, DescriptorHeapType HeapType);
		DescriptorHandle AllocateRange(Device& device, size_t size, DescriptorHeapRange& range);
		DescriptorHandleGpu GetGpuDescriptorHandle();
		pDescriptorHeap GetDescriptorHeapPointer();
	};

}

#endif
