#ifndef RESOURCES_H
#define RESOURCES_H
#include "PlatformDefine.h"
#include "EngineGpuCommands.h"
#include "DescriptorHeapManager.h"

namespace BruteForce
{
	class GpuResource
	{
	protected:
		size_t m_heap_range_srv_index;
		size_t m_heap_range_uav_index;

		GpuAllocation* m_p_allocation;
		int32_t m_descriptor_index;
		ResourceStates      m_state;
		//float               m_clearColor[4];
	public:
		Resource            m_GpuBuffer;

		GpuResource() : m_p_allocation(nullptr), m_descriptor_index(-1) {};
		GpuResource(const GpuResource&) = default;
		~GpuResource()
		{
			if (m_p_allocation)
			{
				m_p_allocation->Release();
				m_p_allocation = nullptr;
			}
		};

		virtual void CreateSrv(Device& device, DescriptorHandle& descriptor_handle) {};
		virtual void CreateSrv(Device& device, DescriptorHeapRange& descriptor_range, size_t index) {};
		virtual void CreateUav(Device& device, DescriptorHandle& descriptor_handle) {};
		virtual void CreateUav(Device& device, DescriptorHeapRange& descriptor_range, size_t index) {};

		void SetName(LPCWSTR name);
		void TransitionTo(SmartCommandList& commandlist, ResourceStates dst);

		friend void CreateBufferResource(Device& device,
			GpuResource& pResource,
			const ResourceDesc* rd,
			ClearValue* pClearValue,
			GpuAllocator gpu_allocator
		);
		friend void CreateBufferResource(Device& device,
			GpuResource& pResource,
			size_t numElements, size_t elementSize,
			GpuAllocator gpu_allocator,
			ResourceFlags flags
		);
	};

	void CreateBufferResource(Device& device,
		GpuResource& pResource,
		const ResourceDesc* rd,
		ClearValue* pClearValue,
		GpuAllocator gpu_allocator
	);


    void CreateBufferResource(Device& device,
        pResource* pDestinationResource,
        size_t numElements, size_t elementSize,
        ResourceFlags flags = ResourceFlagsNone
		);

	void CreateBufferResource(Device& device,
		GpuResource& pResource,
		size_t numElements, size_t elementSize,
		GpuAllocator gpu_allocator,
		ResourceFlags flags = ResourceFlagsNone
		);

    void UpdateBufferResource(
        Device& device,
        GraphicsCommandList commandList,
        pResource* pDestinationResource,
        pResource* pIntermediateResource,
        size_t numElements, size_t elementSize, const void* bufferData);

    void UpdateBufferResource(
        Device& device,
        SmartCommandList commandList,
        pResource* pDestinationResource,
        pResource* pIntermediateResource,
        size_t numElements, size_t elementSize, const void* bufferData);
}



#endif
