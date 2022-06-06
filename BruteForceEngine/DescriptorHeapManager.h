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
		size_t m_End;
		std::string m_Name;
		//DescriptorHandle m_CpuHandle;
		DescriptorHeapRange() :m_Start(0), m_End(0), m_Name("Unnamed") {}
		DescriptorHeapRange(size_t start, size_t end, const std::string &name):m_Start(start), m_End(end), m_Name(name) {}
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
		DescriptorHandle AllocateRange(Device& device, size_t size, const std::string& name);
		DescriptorHandleGpu GetGpuDescriptorHandle();
		pDescriptorHeap GetDescriptorHeapPointer();
	};

}

#endif
