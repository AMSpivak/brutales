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
		m_size = size;
	}

	DescriptorHandle DescriptorHeapManager::AllocateRange(Device& device, size_t size, DescriptorHeapRange& range)
	{
		assert(size + m_index < m_size);

		auto handle = m_Heap->GetCPUDescriptorHandleForHeapStart();
		handle.ptr += device->GetDescriptorHandleIncrementSize(m_HeapType) * static_cast<UINT>(m_index);
		range.m_Start = m_index;
		range.m_Size = size;
		m_index += size;
		return handle;
	}

	DescriptorHandleGpu DescriptorHeapManager::GetGpuDescriptorHandle()
	{
		return m_Heap->GetGPUDescriptorHandleForHeapStart();
	}

	pDescriptorHeap DescriptorHeapManager::GetDescriptorHeapPointer()
	{
		return m_Heap.Get();
	}


	void DescriptorHeapRange::Fill(DescriptorRange* range, size_t reg)
	{
		range->Init(m_Type, m_Size, static_cast<UINT>(reg));
		range->OffsetInDescriptorsFromTableStart = m_Start;
	}

}