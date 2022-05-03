#include "RenderTargetTexture.h"
#include "Helpers.h"



namespace BruteForce
{
	namespace Textures
	{
		bool RenderTargetTexture::CreateViews(Device& device, DescriptorHandle& srv_handle, DescriptorHandle& rt_handle)
		{
			if(!m_resource)
			{
				return false;
			}

			device->CreateRenderTargetView(m_resource.Get(), nullptr, rt_handle);
			device->CreateShaderResourceView(m_resource.Get(), nullptr, srv_handle);
			m_rtvDescriptor = rt_handle;
			m_srvDescriptor = srv_handle;
			return true;
		}

		void RenderTargetTexture::Assign(Device& device, int width, int height, TargetFormat format)
		{
			m_clearColor[0] = m_clearColor[1] = m_clearColor[2] = m_clearColor[3] = 0.0f;
			m_format = format;
			ResourceDesc desc = CD3DX12_RESOURCE_DESC::Tex2D(m_format,
				static_cast<UINT64>(width),
				static_cast<UINT>(height),
				1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

			D3D12_CLEAR_VALUE clearValue = { m_format, {} };
			memcpy(clearValue.Color, m_clearColor, sizeof(clearValue.Color));

			m_state = ResourceStatesRenderTarget;
			auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
			// Create a render target
			ThrowIfFailed(
				device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_ALLOW_ALL_BUFFERS_AND_TEXTURES,
					&desc,
					m_state, &clearValue,
					IID_PPV_ARGS(&m_resource))
			);
			m_resource->SetName(L"RTTexture");
		}
		void RenderTargetTexture::TransitionTo(SmartCommandList& commandlist, ResourceStates dst)
		{
			if(m_state == dst)
			{
				return;
			}

			ResourceBarrier barrier = BruteForce::ResourceBarrier::Transition(
				m_resource.Get(),
				m_state,
				dst);

			commandlist.command_list->ResourceBarrier(1, &barrier);

			m_state = dst;
		}
	}
}
