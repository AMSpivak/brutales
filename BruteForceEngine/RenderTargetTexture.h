#ifndef BRUTEFORCE_RENDERTARGETTEXTURE_H
#define BRUTEFORCE_RENDERTARGETTEXTURE_H
#include "PlatformDefine.h"
#include "EngineGpuCommands.h"

namespace BruteForce
{
	namespace Textures
	{
		class RenderTargetTexture
		{
		private:
			Resource            m_resource;
			ResourceStates      m_state;
			DescriptorHandle    m_srvDescriptor;
			DescriptorHandle    m_rtvDescriptor;

			TargetFormat        m_format;

			size_t              m_width;
			size_t              m_height;
			float               m_clearColor[4];
		public:

			RenderTargetTexture() = default;
			RenderTargetTexture(const RenderTargetTexture&) = default;
			~RenderTargetTexture() {};

			bool CreateViews(Device& device, DescriptorHandle& srv_handle, DescriptorHandle& rt_handle);
			void Assign(Device& device, int width, int height, TargetFormat format);
			//bool BeginRender();
			//bool EndRender();
			void TransitionTo(SmartCommandList& commandlist, ResourceStates dst);
			DescriptorHandle& GetRT();
			DescriptorHandle* GetSrvPointer();

		};

	}
}

#endif
