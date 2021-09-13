#include "Resources.h"
#include "Helpers.h"


namespace BruteForce
{
    void UpdateBufferResource(
        Device& device,
        GraphicsCommandList commandList,
        pResource* pDestinationResource,
        pResource* pIntermediateResource,
        size_t numElements, size_t elementSize, const void* bufferData,
        ResourceFlags flags)
    {
        size_t bufferSize = numElements * elementSize;

        CD3DX12_HEAP_PROPERTIES hp(D3D12_HEAP_TYPE_DEFAULT);
        CD3DX12_RESOURCE_DESC rd = CD3DX12_RESOURCE_DESC::Buffer(bufferSize, flags);
        ThrowIfFailed(device->CreateCommittedResource(
            &hp,
            D3D12_HEAP_FLAG_NONE,
            &rd,
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(pDestinationResource)));

        if (bufferData)
        {
            CD3DX12_HEAP_PROPERTIES hp2(D3D12_HEAP_TYPE_UPLOAD);
            CD3DX12_RESOURCE_DESC rd2 = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

            ThrowIfFailed(device->CreateCommittedResource(
                &hp2,
                D3D12_HEAP_FLAG_NONE,
                &rd2,
                D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(pIntermediateResource)));

            D3D12_SUBRESOURCE_DATA subresourceData = {};
            subresourceData.pData = bufferData;
            subresourceData.RowPitch = bufferSize;
            subresourceData.SlicePitch = subresourceData.RowPitch;

            UpdateSubresources(commandList.Get(),
                *pDestinationResource,
                *pIntermediateResource,
                0, 0, 1, &subresourceData);
        }
    }

    void UpdateBufferResource(
        Device& device,
        SmartCommandList commandList,
        pResource* pDestinationResource,
        pResource* pIntermediateResource,
        size_t numElements, size_t elementSize, const void* bufferData,
        ResourceFlags flags)
    {
        UpdateBufferResource(device, commandList.command_list, pDestinationResource, pIntermediateResource,
            numElements, elementSize, bufferData, flags);
    }
}