#ifndef CONSTANT_BUFFER_PLAT_H
#define CONSTANT_BUFFER_PLAT_H

#include "PlatformDefine.h"
#include "Helpers.h"
#ifdef _DEBUG
#include <typeinfo>
#include <windows.h>
#endif
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
#ifdef _DEBUG
        wchar_t* wString = new wchar_t[4096];
        MultiByteToWideChar(CP_ACP, 0, typeid(buffer).name(), -1, wString, 4096);
        buffer.m_GpuBuffer->SetName(wString);
#else
        buffer.m_GpuBuffer->SetName(L"Constant Buffer Upload Resource Heap");
#endif
        D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
        cbvDesc.BufferLocation = buffer.m_GpuBuffer->GetGPUVirtualAddress();
        cbvDesc.SizeInBytes = static_cast<UINT>(buffer.GetBufferSize());
        device->CreateConstantBufferView(&cbvDesc, cb_handle);
    }

    template <typename CBStruct>
    void MapUploadCB(CBStruct& buffer)
    {
        CD3DX12_RANGE readRange(0, 0);    // We do not intend to read from this resource on the CPU. (End is less than or equal to begin)
        ThrowIfFailed(buffer.m_GpuBuffer->Map(0, &readRange, reinterpret_cast<void**>(&buffer.m_GpuAddress)));
    }


#endif
}


#endif
