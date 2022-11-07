#include "RWRawBuffer.h"

namespace BruteForce
{
	namespace Buffers
	{
		void RWRawBuffer::Create(Device& device, size_t size, GpuAllocator gpu_allocator)
		{
			ResourceDesc res_desc = CD3DX12_RESOURCE_DESC::Buffer(size, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

			CreateBufferResource(device, *this, &res_desc, nullptr, gpu_allocator);
			m_size = size;
		}
		/*void RWRawBuffer::CreateSrv(Device& device, DescriptorHeapRange& descriptor_range, size_t index)
		{
		}*/
		void RWRawBuffer::CreateUav(Device& device, DescriptorHandle& descriptor_handle)
		{
			D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
			uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
			uavDesc.Format = DXGI_FORMAT_R32_TYPELESS;
			uavDesc.Buffer.NumElements = m_size / 4;
			uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_RAW;
			device->CreateUnorderedAccessView(m_GpuBuffer.Get(), nullptr, &uavDesc, descriptor_handle);
			//m_descriptor_handle = descriptor_handle;
		}
		void RWRawBuffer::CreateUav(Device& device, DescriptorHeapRange& descriptor_range, size_t index)
		{
			assert(index < descriptor_range.m_Size);
			m_heap_range_uav_index = index;
			auto descriptor_handle = descriptor_range.m_CpuHandle;
			descriptor_handle.ptr += device->GetDescriptorHandleIncrementSize(BruteForce::DescriptorHeapCvbSrvUav) * m_heap_range_uav_index;
			CreateUav(device, descriptor_handle);
		}
	}
}