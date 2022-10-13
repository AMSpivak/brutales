#include "Resources.h"
#include "Helpers.h"


namespace BruteForce
{

    void GpuResource::SetName(LPCWSTR name)
    {
        m_GpuBuffer->SetName(name);
    }

    void GpuResource::TransitionTo(SmartCommandList& commandlist, ResourceStates dst)
    {
        if (m_state == dst)
        {
            return;
        }

        ResourceBarrier barrier = BruteForce::ResourceBarrier::Transition(
            m_GpuBuffer.Get(),
            m_state,
            dst);

        commandlist.command_list->ResourceBarrier(1, &barrier);

        m_state = dst;
    }




    void CreateBufferResource(Device& device,
        pResource* pDestinationResource,
        size_t numElements, size_t elementSize,
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
    }

    void UpdateBufferResource(
        Device& device,
        GraphicsCommandList commandList,
        pResource* pDestinationResource,
        pResource* pIntermediateResource,
        size_t numElements, size_t elementSize, const void* bufferData)
    {
        size_t bufferSize = numElements * elementSize;

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
        size_t numElements, size_t elementSize, const void* bufferData)
    {
        UpdateBufferResource(device, commandList.command_list, pDestinationResource, pIntermediateResource,
            numElements, elementSize, bufferData);
    }
}