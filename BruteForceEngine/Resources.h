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

		Resource            m_resource;
		GpuAllocation* m_p_allocation;
		ResourceStates      m_state;
		//float               m_clearColor[4];
		bool				m_render_target;
	public:
		GpuResource() : m_p_allocation(nullptr) {};
		GpuResource(const GpuResource&) = default;
		~GpuResource()
		{
			if (m_p_allocation)
			{
				m_p_allocation->Release();
				m_p_allocation = nullptr;
			}
		};

		virtual void CreateSrv(Device& device, DescriptorHandle& descriptor_handle) = 0;
		virtual void CreateSrv(Device& device, DescriptorHeapRange& descriptor_range, size_t index) = 0;
		virtual void CreateUav(Device& device, DescriptorHandle& descriptor_handle) = 0;
		virtual void CreateUav(Device& device, DescriptorHeapRange& descriptor_range, size_t index) = 0;

		void SetName(LPCWSTR name);
		void TransitionTo(SmartCommandList& commandlist, ResourceStates dst);
	};



    void CreateBufferResource(Device& device,
        pResource* pDestinationResource,
        size_t numElements, size_t elementSize,
        ResourceFlags flags = ResourceFlagsNone);

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
