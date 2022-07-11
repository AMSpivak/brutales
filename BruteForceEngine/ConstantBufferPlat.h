#ifndef CONSTANT_BUFFER_PLAT_H
#define CONSTANT_BUFFER_PLAT_H

#include "PlatformDefine.h"

namespace BruteForce
{
#ifdef PLATFORM_DX12_H

    template <typename CBStruct>
    void CreateUploadGPUBuffer(Device& device, CBStruct& buffer, DescriptorHandle cb_handle)
    {
        auto heap_props = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        auto res_desc = CD3DX12_RESOURCE_DESC::Buffer(buffer.GetResourceHeapSize());
        ThrowIfFailed(device->CreateCommittedResource(
            &heap_props,
            HeapFlagsNone,
            &res_desc,
            ResourceStateRead,
            nullptr,
            IID_PPV_ARGS(&buffer.m_GpuBuffer)));

        buffer.m_GpuBuffer->SetName(L"Constant Buffer Upload Resource Heap");
        D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
        cbvDesc.BufferLocation = buffer.m_GpuBuffer->GetGPUVirtualAddress();
        cbvDesc.SizeInBytes = static_cast<UINT>(buffer.GetBufferSize());
        device->CreateConstantBufferView(&cbvDesc, cb_handle);
    }

#endif
}


#endif
