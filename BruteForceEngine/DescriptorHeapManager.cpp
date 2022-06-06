#include "DescriptorHeapManager.h"

namespace BruteForce
{
	void DescriptorHeapManager::Create(Device& device, size_t size, DescriptorHeapType HeapType)
	{
		BruteForce::DescriptorHeapDesc descHeap = {};
		descHeap.NumDescriptors = static_cast<UINT>(size);
		descHeap.Type = HeapType;
		descHeap.Flags = DescriptorHeapShaderVisible;
		ThrowIfFailed(device->CreateDescriptorHeap(&descHeap, __uuidof(ID3D12DescriptorHeap), (void**)&m_Heap));
		m_Heap->SetName(L"Managed descriptor heap");
	}

	DescriptorHandle DescriptorHeapManager::AllocateRange(size_t size)
	{
		return DescriptorHandle();
	}
}