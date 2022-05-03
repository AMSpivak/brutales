#include "DepthBuffer.h"
#include "PlatformDefine.h"
#include "Helpers.h"

namespace BruteForce
{
	namespace Textures
	{
		bool DepthBuffer::CreateSrv(Device& device, DescriptorHandle& descriptor_handle)
		{
			if(m_buffer)
			{
				D3D12_DEPTH_STENCIL_VIEW_DESC dsv = {};
				dsv.Format = m_format;
				dsv.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
				dsv.Texture2D.MipSlice = 0;
				dsv.Flags = D3D12_DSV_FLAG_NONE;

				device->CreateDepthStencilView(m_buffer.Get(), &dsv,
					descriptor_handle);
				return true;
			}
			else
			{
				return false;
			}
		}
		void DepthBuffer::Assign(Device& device, int width, int height, TargetFormat format)
		{
			D3D12_CLEAR_VALUE optimizedClearValue = {};
			optimizedClearValue.Format = format;
			optimizedClearValue.DepthStencil = { 1.0f, 0 };
			BruteForce::HeapProperties hp{ BruteForce::HeapTypeDefault };
			BruteForce::CResourceDesc rd = BruteForce::CResourceDesc::Tex2D(optimizedClearValue.Format, width, height,
				1, 0, 1, 0, BruteForce::ResourceFlagsDepthStencil);
			ThrowIfFailed(device->CreateCommittedResource(
				&hp,
				BruteForce::HeapFlagsNone,
				&rd,
				BruteForce::ResourceStateDepthWrite,
				&optimizedClearValue,
				IID_PPV_ARGS(&m_buffer)
			));
			m_format = format;
		}
	}
}